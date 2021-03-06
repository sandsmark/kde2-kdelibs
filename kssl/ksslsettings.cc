/* This file is part of the KDE project
 *
 * Copyright (C) 2000 George Staikos <staikos@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
 
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>

#include <qfile.h>
#include <qsortedlist.h>

#include "ksslsettings.h"
#include <kglobal.h>
#include <kstddirs.h>
#include <kdebug.h>

// this hack provided by Malte Starostik to avoid glibc/openssl bug
// on some systems
#ifdef HAVE_SSL
#define crypt _openssl_crypt
#include <openssl/ssl.h>
#undef crypt
#endif

#include <kopenssl.h>

      class CipherNode {
      public:
        CipherNode(const char *_name, int _keylen) : 
                      name(_name), keylen(_keylen) {}
        QString name;
        int keylen;
        inline int operator==(CipherNode &x) 
                     { return ((x.keylen == keylen) && (x.name == name)); }
        inline int operator< (CipherNode &x) { return keylen < x.keylen;  }
        inline int operator<=(CipherNode &x) { return keylen <= x.keylen; }
        inline int operator> (CipherNode &x) { return keylen > x.keylen;  }
        inline int operator>=(CipherNode &x) { return keylen >= x.keylen; }
      };


class KSSLSettingsPrivate {
public:
  KSSLSettingsPrivate() {
    kossl = KOSSL::self();
  }
  ~KSSLSettingsPrivate() {

  }

  KOSSL *kossl;
  bool m_bUseEGD;
  QString m_EGDPath;
  bool m_bSendX509;
  bool m_bPromptX509;
};

//
// FIXME
// Implementation note: for now, we only read cipher settings from disk,
//                      and do not store them in memory.  This should change.
//

KSSLSettings::KSSLSettings(bool readConfig) {
  d = new KSSLSettingsPrivate;
  m_cfg = new KConfig("cryptodefaults", false, false);

  if (!KGlobal::dirs()->addResourceType("kssl", "share/apps/kssl")) {
    //kdDebug(7029) << "Error adding (kssl, share/apps/kssl)" << endl;
  }

  if (readConfig) load();
}


// we don't save settings incase it was a temporary object
KSSLSettings::~KSSLSettings() {
  delete m_cfg;
  delete d;
}
 

bool KSSLSettings::sslv2() const {
  return m_bUseSSLv2;
}


bool KSSLSettings::sslv3() const {
  return m_bUseSSLv3;
}


bool KSSLSettings::tlsv1() const {
  return m_bUseTLSv1;
}

 
// FIXME: we should make a default list available if this fails
//        since OpenSSL seems to just choose any old thing if it's given an
//        empty list.  This behaviour is not confirmed though.
QString KSSLSettings::getCipherList() {
QString clist = "";
#ifdef HAVE_SSL
    QString tcipher;
    bool firstcipher = true;
    SSL_METHOD *meth;
    QSortedList<CipherNode> cipherSort;
    cipherSort.setAutoDelete(true);


    if (m_bUseSSLv2 && m_bUseSSLv3)
        meth = d->kossl->TLS_client_method();
    else if (m_bUseSSLv3)
        meth = d->kossl->SSLv3_client_method();
    else
        meth = d->kossl->SSLv2_client_method();

    SSL_CTX *ctx = d->kossl->SSL_CTX_new(meth);
    SSL* ssl = d->kossl->SSL_new(ctx);
    STACK_OF(SSL_CIPHER)* sk = d->kossl->SSL_get_ciphers(ssl);
    int cnt = d->kossl->sk_num(sk);
    for (int i=0; i< cnt; i++) {
        SSL_CIPHER *sc = reinterpret_cast<SSL_CIPHER*>(d->kossl->sk_value(sk,i));
        if (!sc) break;

        if(!strcmp("SSLv2", d->kossl->SSL_CIPHER_get_version(sc))) {
            m_cfg->setGroup("SSLv2");
        } else {
            m_cfg->setGroup("SSLv3");

            tcipher.sprintf("cipher_%s", d->kossl->SSL_CIPHER_get_name(sc));
            int bits = d->kossl->SSL_CIPHER_get_bits(sc, NULL);

            if (m_cfg->readBoolEntry(tcipher, bits >= 56)) {
                CipherNode *xx = new CipherNode(d->kossl->SSL_CIPHER_get_name(sc),bits);
                if (!cipherSort.contains(xx))
                    cipherSort.inSort(xx);
                else delete xx;
            } // if
        } // for   i
    }
    d->kossl->SSL_free(ssl);
    d->kossl->SSL_CTX_free(ctx);

    // Hack time
    // ---------
    //    A lot of these webservers suck.  So in order to get around their
    // sucking, we take the most common ciphers and make them the first ones
    // we offer.  This seems to make it work better.
    //

    // Put least preferred first, most preferred last.
    CipherNode tnode("", 0);

#define AdjustCipher(X, Y)    tnode.name = X;  tnode.keylen = Y;             \
    if (cipherSort.find(&tnode) != -1) {                                     \
       cipherSort.remove();                                                  \
       cipherSort.append(new CipherNode(tnode.name.latin1(), tnode.keylen)); \
    }

    AdjustCipher("IDEA-CBC-MD5", 128);
    AdjustCipher("DES-CBC3-MD5", 168);
    AdjustCipher("RC2-CBC-MD5", 128);
    AdjustCipher("DES-CBC3-SHA", 168);
    AdjustCipher("IDEA-CBC-SHA", 128);
    AdjustCipher("RC4-SHA", 128);
    AdjustCipher("RC4-MD5", 128);
#undef AdjustCipher


    // now assemble the list  cipher1:cipher2:cipher3:...:ciphern
    while (!cipherSort.isEmpty()) {
      if (firstcipher)
        firstcipher = false;
      else clist.append(":");
      clist.append(cipherSort.getLast()->name);
      cipherSort.removeLast();
    } // while
    
    //    kdDebug(7029) << "Cipher list is: " << clist << endl;

#endif
return clist;
}

// FIXME - sync these up so that we can use them with the control module!!
void KSSLSettings::load() {
  m_cfg->reparseConfiguration();

  m_cfg->setGroup("TLS");
  m_bUseTLSv1 = m_cfg->readBoolEntry("Enabled", true);
 
  m_cfg->setGroup("SSLv2");
  m_bUseSSLv2 = m_cfg->readBoolEntry("Enabled", true);
 
  m_cfg->setGroup("SSLv3");
  m_bUseSSLv3 = m_cfg->readBoolEntry("Enabled", true);
 
  m_cfg->setGroup("Warnings");
  m_bWarnOnEnter = m_cfg->readBoolEntry("OnEnter", false);
  m_bWarnOnLeave = m_cfg->readBoolEntry("OnLeave", true);
  m_bWarnOnUnencrypted = m_cfg->readBoolEntry("OnUnencrypted", false);
  m_bWarnOnMixed = m_cfg->readBoolEntry("OnMixed", true);

  m_cfg->setGroup("Validation");
  m_bWarnSelfSigned = m_cfg->readBoolEntry("WarnSelfSigned", true);
  m_bWarnExpired = m_cfg->readBoolEntry("WarnExpired", true);
  m_bWarnRevoked = m_cfg->readBoolEntry("WarnRevoked", true);

  m_cfg->setGroup("EGD");
  d->m_bUseEGD = m_cfg->readBoolEntry("UseEGD", false);
  d->m_EGDPath = m_cfg->readEntry("EGDPath");

  m_cfg->setGroup("Auth");
  d->m_bSendX509 = ("send" == m_cfg->readEntry("AuthMethod", ""));
  d->m_bPromptX509 = ("prompt" == m_cfg->readEntry("AuthMethod", ""));

  #ifdef HAVE_SSL



  #endif
}


void KSSLSettings::defaults() {
  m_bUseTLSv1 = true;
  m_bUseSSLv2 = true;
  m_bUseSSLv3 = true;
  m_bWarnOnEnter = false;
  m_bWarnOnLeave = true;
  m_bWarnOnUnencrypted = false;
  m_bWarnOnMixed = true;
  m_bWarnSelfSigned = true;
  m_bWarnExpired = true;
  m_bWarnRevoked = true;
  d->m_bUseEGD = false;
  d->m_EGDPath = "";
}


void KSSLSettings::save() {
  m_cfg->setGroup("TLS");
  m_cfg->writeEntry("Enabled", m_bUseTLSv1);
 
  m_cfg->setGroup("SSLv2");
  m_cfg->writeEntry("Enabled", m_bUseSSLv2);
 
  m_cfg->setGroup("SSLv3");
  m_cfg->writeEntry("Enabled", m_bUseSSLv3);
 
  m_cfg->setGroup("Warnings");
  m_cfg->writeEntry("OnEnter", m_bWarnOnEnter);
  m_cfg->writeEntry("OnLeave", m_bWarnOnLeave);
  m_cfg->writeEntry("OnUnencrypted", m_bWarnOnUnencrypted);
  m_cfg->writeEntry("OnMixed", m_bWarnOnMixed);

  m_cfg->setGroup("Validation");
  m_cfg->writeEntry("WarnSelfSigned", m_bWarnSelfSigned);
  m_cfg->writeEntry("WarnExpired", m_bWarnExpired);
  m_cfg->writeEntry("WarnRevoked", m_bWarnRevoked);

  m_cfg->setGroup("EGD");
  m_cfg->writeEntry("UseEGD", d->m_bUseEGD);
  m_cfg->writeEntry("EGDPath", d->m_EGDPath);

  // FIXME - ciphers
#if 0
#ifdef HAVE_SSL
  m_cfg->setGroup("SSLv2");
  for (unsigned int i = 0; i < v2ciphers.count(); i++) {
    QString ciphername;
    ciphername.sprintf("cipher_%s", v2ciphers[i].ascii());
    if (v2selectedciphers.contains(v2ciphers[i])) {
      m_cfg->writeEntry(ciphername, true);
    } else m_cfg->writeEntry(ciphername, false);
  }
 
  m_cfg->setGroup("SSLv3");
  for (unsigned int i = 0; i < v3ciphers.count(); i++) {
    QString ciphername;
    ciphername.sprintf("cipher_%s", v3ciphers[i].ascii());
    if (v3selectedciphers.contains(v3ciphers[i])) {
      m_cfg->writeEntry(ciphername, true);
    } else m_cfg->writeEntry(ciphername, false);
  }
#endif 
 
  m_cfg->sync();
 
  // insure proper permissions -- contains sensitive data
  QString cfgName(KGlobal::dirs()->findResource("config", "cryptodefaults"));
  if (!cfgName.isEmpty())
    ::chmod(QFile::encodeName(cfgName), 0600);
#endif
}


bool KSSLSettings::warnOnEnter() const       { return m_bWarnOnEnter; }
bool KSSLSettings::warnOnUnencrypted() const { return m_bWarnOnUnencrypted; }
bool KSSLSettings::warnOnLeave() const       { return m_bWarnOnLeave; }
bool KSSLSettings::warnOnMixed() const       { return m_bWarnOnMixed; }
bool KSSLSettings::warnOnSelfSigned() const  { return m_bWarnSelfSigned; }
bool KSSLSettings::warnOnRevoked() const     { return m_bWarnRevoked; }
bool KSSLSettings::warnOnExpired() const     { return m_bWarnExpired; }
bool KSSLSettings::useEGD() const            { return d->m_bUseEGD;      }
bool KSSLSettings::autoSendX509() const      { return d->m_bSendX509; }
bool KSSLSettings::promptSendX509() const    { return d->m_bPromptX509; }

void KSSLSettings::setTLSv1(bool enabled) { m_bUseTLSv1 = enabled; }
void KSSLSettings::setSSLv2(bool enabled) { m_bUseSSLv2 = enabled; }
void KSSLSettings::setSSLv3(bool enabled) { m_bUseSSLv3 = enabled; }

QString& KSSLSettings::getEGDPath()       { return d->m_EGDPath; }

