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

// this hack provided by Malte Starostik to avoid glibc/openssl bug
// on some systems
#ifdef HAVE_SSL
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define crypt _openssl_crypt
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#undef crypt
#endif

#include "kssl.h"

#include <kdebug.h>
#include <kstddirs.h>
#include <ksock.h>
#include <ksockaddr.h>

class KSSLPrivate {
public:
  KSSLPrivate() {
    lastInitTLS = false;
  }

  ~KSSLPrivate() {

  }

  bool lastInitTLS;
  KSSLCertificate::KSSLValidation m_cert_vfy_res;
  #ifdef HAVE_SSL
    SSL *m_ssl;
    SSL_CTX *m_ctx;
    SSL_METHOD *m_meth;
  #endif
};


KSSL::KSSL(bool init) {
  d = new KSSLPrivate;
  m_bInit = false;
  m_bAutoReconfig = true;
#ifdef HAVE_SSL
  OpenSSL_add_ssl_algorithms();
  OpenSSL_add_all_algorithms();
#endif
  m_cfg = new KSSLSettings();

 if (init) initialize();
}


KSSL::~KSSL() {
  close();
  delete m_cfg;
  delete d;
}


int KSSL::seedWithEGD() {
int rc = 0;
  #ifdef HAVE_SSL
  if (m_cfg->useEGD() && !m_cfg->getEGDPath().isEmpty()) {
    rc = RAND_egd(m_cfg->getEGDPath().latin1());
    if (rc < 0) 
      kdDebug() << "KSSL: Error seeding PRNG with the EGD." << endl;
    else 
      kdDebug() << "KSSL: PRNG was seeded with " << rc 
                << " bytes from the EGD." << endl;
  }
  #endif
  return rc;
}


bool KSSL::TLSInit() {
#ifdef HAVE_SSL
  // kdDebug() << "KSSL TLS initialize" << endl;
  if (m_bInit) return false;

  if (m_bAutoReconfig)
    m_cfg->load();

  seedWithEGD();
  d->m_meth = TLSv1_client_method();
  d->lastInitTLS = true;

  OpenSSL_add_ssl_algorithms();
  OpenSSL_add_all_algorithms();
  d->m_ctx=SSL_CTX_new(d->m_meth);
  if (d->m_ctx == NULL) {
    return false;
  }

  // set cipher list
  QString clist = m_cfg->getCipherList();
  if (!clist.isEmpty())
    SSL_CTX_set_cipher_list(d->m_ctx, const_cast<char *>(clist.ascii()));

  m_bInit = true;
return true;
#else
return false;
#endif
}


bool KSSL::initialize() {
#ifdef HAVE_SSL
  kdDebug() << "KSSL initialize" << endl;
  if (m_bInit) return false;

  if (m_bAutoReconfig)
    m_cfg->load();

  seedWithEGD();
  // TLS1 goes first - it excludes SSL2/3
  // FIXME: we should be able to force SSL off entirely.
  //        This logic here makes v2 a "default" if no other SSL
  //        version is turned on.  IMHO this is the safest one to
  //        use as the default anyways, so I'm not changing it yet.
  d->lastInitTLS = m_cfg->tlsv1();

  if (m_cfg->tlsv1())
    d->m_meth = TLSv1_client_method();
  else if (m_cfg->sslv2() && m_cfg->sslv3())
    d->m_meth = SSLv23_client_method();
  else if (m_cfg->sslv3())
    d->m_meth = SSLv3_client_method();
  else
    d->m_meth = SSLv2_client_method();

  /*
  if (m_cfg->sslv2() && m_cfg->sslv3()) kdDebug() << "Double method" << endl;
  else if (m_cfg->sslv2()) kdDebug() << "SSL2 method" << endl;
  else if (m_cfg->sslv3()) kdDebug() << "SSL3 method" << endl;
  */

  d->m_ctx=SSL_CTX_new(d->m_meth);
  if (d->m_ctx == NULL) {
    return false;
  }

  // set cipher list
  QString clist = m_cfg->getCipherList();
  if (!clist.isEmpty())
    SSL_CTX_set_cipher_list(d->m_ctx, const_cast<char *>(clist.ascii()));

  m_bInit = true;
return true;
#else
return false;
#endif
}




void KSSL::close() {
#ifdef HAVE_SSL
  // kdDebug() << "KSSL close" << endl;
  if (!m_bInit) return;
  SSL_shutdown(d->m_ssl);
  SSL_free(d->m_ssl);
  SSL_CTX_free(d->m_ctx);
  m_bInit = false;
#endif
}


bool KSSL::reInitialize() {
  close();
  return initialize();
}

// get the callback file - it's hidden away in here
#include "ksslcallback.c"


bool KSSL::setVerificationLogic() {
#if 0
#ifdef HAVE_SSL
  //  SSL_set_verify_result(d->m_ssl, X509_V_OK);
  //  SSL_CTX_set_verify(d->m_ctx, SSL_VERIFY_PEER, X509Callback);
#endif
#endif
  return true;
}


int KSSL::connect(int sock) {
#ifdef HAVE_SSL
  // kdDebug() << "KSSL connect" << endl;
  int rc;
  if (!m_bInit) return -1;
  d->m_ssl = SSL_new(d->m_ctx);
  if (!d->m_ssl) return -1;

  if (!setVerificationLogic())
    return -1;

  if (!d->lastInitTLS)
    SSL_set_options(d->m_ssl, SSL_OP_NO_TLSv1);

  rc = SSL_set_fd(d->m_ssl, sock);
  if (rc == 0) return rc;

  rc = SSL_connect(d->m_ssl);
  if (rc == 1) {
    setConnectionInfo();
    setPeerInfo(sock);
    // kdDebug() << "KSSL connected OK" << endl;
  } else {
    if (m_cfg->sslv2() && m_cfg->sslv3()) {
      m_cfg->setSSLv2(true);
      m_cfg->setSSLv3(false);
      m_bAutoReconfig = false;
      m_bInit = false;
      SSL_CTX_free(d->m_ctx);
      kdDebug() << "KSSL connecting again" << endl;
      initialize();
      rc = KSSL::connect(sock);
      if (rc == 1) {
	setConnectionInfo();
	setPeerInfo(sock);
      } else {
	m_cfg->setSSLv3(true);
	m_bAutoReconfig = true;
	kdDebug() << "KSSL connect FAILED" << endl;
	return -1;
      }
    }
  }
  return rc;
#else
  return -1;
#endif
}


int KSSL::read(void *buf, int len) {
#ifdef HAVE_SSL
  if (!m_bInit) return -1;
  return SSL_read(d->m_ssl, (char *)buf, len);
#else
  return -1;
#endif
}


int KSSL::write(const void *buf, int len) {
#ifdef HAVE_SSL
  if (!m_bInit) return -1;
  return SSL_write(d->m_ssl, (const char *)buf, len);
#else
  return -1;
#endif
}


bool KSSL::reconfig() {
  return reInitialize();
}


void KSSL::setAutoReconfig(bool ar) {
  m_bAutoReconfig = ar;
}


bool KSSL::setSettings(KSSLSettings *settings) {
  delete m_cfg;
  m_cfg = settings;
  return reconfig();
}


#ifdef HAVE_SSL
bool KSSL::m_bSSLWorks = true;
#else
bool KSSL::m_bSSLWorks = false;
#endif

bool KSSL::doesSSLWork() {
  return m_bSSLWorks;
}


void KSSL::setConnectionInfo() {
#ifdef HAVE_SSL
  SSL_CIPHER *sc;
  char buf[1024];

  buf[0] = 0;  // for safety.
  sc = SSL_get_current_cipher(d->m_ssl);
  if (!sc) {
   kdDebug() << "KSSL get current cipher failed - we're probably gonna crash!" << endl;
  return;
  }
  // set the number of bits, bits used
  m_ci.m_iCipherUsedBits = SSL_CIPHER_get_bits(sc, &(m_ci.m_iCipherBits));
  // set the cipher version
  m_ci.m_cipherVersion = SSL_CIPHER_get_version(sc);
  // set the cipher name
  m_ci.m_cipherName = SSL_CIPHER_get_name(sc);
  // set the cipher description
  m_ci.m_cipherDescription = SSL_CIPHER_description(sc, buf, 1023);

#endif
}


void KSSL::setPeerInfo(int sock) {
#ifdef HAVE_SSL
// FIXME: Set the right value here
//                          d->m_cert_vfy_res);
  ksockaddr_in sa;
  ksocklen_t nl = sizeof(ksockaddr_in);
  int rc = getpeername (sock, (sockaddr *)&sa, &nl);

  if (rc != -1) {
    QString haddr;
    Q_UINT32 iaddr = ntohl(sa.sin_addr.s_addr);
    haddr.sprintf("%u.%u.%u.%u", (iaddr >> 24) &0xff,
                                 (iaddr >> 16) &0xff,
                                 (iaddr >> 8)  &0xff,
                                 (iaddr)       &0xff);
    m_pi.setPeerAddress(haddr);
    m_pi.setPeerIP(iaddr);
  }
  m_pi.m_cert.setCert(SSL_get_peer_certificate(d->m_ssl));
#endif
}


KSSLConnectionInfo& KSSL::connectionInfo() {
  return m_ci;
}


KSSLPeerInfo& KSSL::peerInfo() {
  return m_pi;
}


bool KSSL::setClientCertificate(KSSLCertificate *cert) {
#ifdef HAVE_SSL
  int rc;

  rc = SSL_CTX_use_certificate(d->m_ctx, cert->getCert());

  if (rc != 0) {
    return false;
  }

  return true;
#else
  return false;
#endif
}
