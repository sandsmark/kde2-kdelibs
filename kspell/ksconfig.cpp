// $Id$

#include <qcheckbox.h>
#include <qfiledialog.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include <kapp.h>
#include <klocale.h>
#include <kdebug.h>
#include <klined.h>

#include "ksconfig.h"
#include <kglobal.h>

KSpellConfig::KSpellConfig (const KSpellConfig &_ksc)
	: QWidget(0, 0)
{  
  setNoRootAffix (_ksc.noRootAffix());
  setRunTogether (_ksc.runTogether());
  setDictionary  (_ksc.dictionary());
  setDictFromList (_ksc.dictFromList());
  //  setPersonalDict (_ksc.personalDict());
  setIgnoreList (_ksc.ignoreList());
  setEncoding (_ksc.encoding());
  setClient (_ksc.client());

  nodialog=TRUE;
}

KSpellConfig::KSpellConfig (QWidget *parent, const char *name,
			    KSpellConfig *_ksc) : QWidget (parent, name)
{
  kc=KGlobal::config();

  if (_ksc==0)
    {
      //I'd like to read these from a kspellrc file so that we could have
      //_adjustable_ system-wide defaults.

      //Here it is.
      readGlobalSettings();
    }
  else
    {
      setNoRootAffix (_ksc->noRootAffix());
      setRunTogether (_ksc->runTogether());
      setDictionary  (_ksc->dictionary());
      setDictFromList (_ksc->dictFromList());
      //      setPersonalDict (_ksc->personalDict());
      setIgnoreList (_ksc->ignoreList());
      setEncoding (_ksc->encoding());
      setClient (_ksc->client());
    }

  nodialog=FALSE;

  //  if (parent!=0)
    {
      //From dlgedit

      layout = new QGridLayout (this, 6, 3, 10, 1);

      cb1 = new QCheckBox(i18n("Create root/&affix combinations"
			       " not in dictionary"), this);
      connect( cb1, SIGNAL(toggled(bool)), SLOT(sNoAff(bool)) );
      cb1->setMinimumSize (cb1->sizeHint());
      layout->addMultiCellWidget (cb1, 0, 0, 0, 2);

      cb2 = new QCheckBox( i18n("Consider &run-together words"
				" as spelling errors"), this );
      connect( cb2, SIGNAL(toggled(bool)), SLOT(sRunTogether(bool)) );
      cb2->setMinimumSize (cb2->sizeHint());
      layout->addMultiCellWidget (cb2, 1, 1, 0, 2);

      dictcombo = new QComboBox (this);
      dictcombo->setInsertionPolicy (QComboBox::NoInsertion);
      dictcombo->setMinimumSize (dictcombo->sizeHint());
      connect (dictcombo, SIGNAL (activated (int)),
	       this, SLOT (sSetDictionary (int)));
      layout->addMultiCellWidget (dictcombo, 2, 2, 1, 2);

      dictlist=new QLabel (dictcombo, i18n("&Dictionary:"), this);
      dictlist->setAlignment (AlignVCenter);
      dictlist->setMinimumSize (dictlist->sizeHint());
      layout->addWidget (dictlist, 2 ,0);

      encodingcombo = new QComboBox (FALSE, this);
      encodingcombo->insertItem (i18n("7-Bit/ASCII"));
      encodingcombo->insertItem (i18n("Latin1"));
      encodingcombo->insertItem (i18n("Latin2"));
      encodingcombo->setMinimumSize (encodingcombo->sizeHint());
      connect (encodingcombo, SIGNAL (activated(int)), this,
	       SLOT (sChangeEncoding(int)));
      layout->addMultiCellWidget (encodingcombo, 3, 3, 1, 2);


      QLabel* tmpQLabel;
      tmpQLabel = new QLabel( encodingcombo, i18n("&Encoding:"), this);
      dictlist->setAlignment (AlignVCenter);
      tmpQLabel->setMinimumSize (tmpQLabel->sizeHint());
      layout->addWidget (tmpQLabel, 3, 0);

      clientcombo = new QComboBox (FALSE, this);
      clientcombo->insertItem (i18n("International Ispell"));
      clientcombo->insertItem (i18n("Aspell"));
      clientcombo->setMinimumSize (clientcombo->sizeHint());
      connect (clientcombo, SIGNAL (activated(int)), this,
	       SLOT (sChangeClient(int)));
      layout->addMultiCellWidget (clientcombo, 4, 4, 1, 2);


      tmpQLabel = new QLabel( clientcombo, i18n("Cl&ient:"), this);
      dictlist->setAlignment (AlignVCenter);
      tmpQLabel->setMinimumSize (tmpQLabel->sizeHint());
      layout->addWidget (tmpQLabel, 4, 0);



      QPushButton* tmpQPushButton;
      tmpQPushButton = new QPushButton( this);
      tmpQPushButton->setText ( i18n("&Help") );
      connect( tmpQPushButton, SIGNAL(clicked()), SLOT(sHelp()) );
      tmpQPushButton->setMinimumSize (tmpQPushButton->sizeHint());
      layout->addWidget (tmpQPushButton, 5, 2);



      layout->activate();
      
      fillInDialog();	







	/* not w/o alternate dict.
	  dictgroup=new QButtonGroup ();
	  dictgroup->setFrameStyle (QFrame::NoFrame);
	  //layout->addWidget (dictgroup,rdictgroup,0);
	  
	dictlistbutton=new QRadioButton (i18n("Language:"),this);
	connect (dictlistbutton, SIGNAL (toggled(bool)),
		 this, SLOT (sDictionary(bool)));
	layout->addWidget (dictlistbutton,rdictlist,0);
	*/

	/* No alternate now -- is this needed?
	dicteditbutton=new QRadioButton (i18n("Alternate Dictionary"),this);
	connect (dicteditbutton, SIGNAL (toggled(bool)),
		 this, SLOT (sPathDictionary(bool)));
	layout->addWidget (dicteditbutton,rdictedit,0);
	dicteditbutton->setMinimumSize (dicteditbutton->
					 sizeHint();
					 */

	/*
	dictgroup->insert (dictlistbutton);
	dictgroup->insert (dicteditbutton);
	*/


	/*	
	tmpQLabel = new QLabel( this, "Label_1" );
	tmpQLabel->setGeometry( 20, 120, 120, 30 );
	tmpQLabel->setText( i18n("Language:"));
	tmpQLabel->setAlignment( 290 );
	tmpQLabel->setMargin( -1 );
	layout->addWidget (tmpQLabel, 3, 1);
	*/

	/*  I'll put this back if peple think that it's necessary,
	    but it would need to be supported better. */
	/*
	tmpQLabel = new QLabel( this, "Label_2" );
	//tmpQLabel->setGeometry( 30, 160, 120, 30 );
	tmpQLabel->setText( i18n("Personal dictionary:") );
	//	tmpQLabel->setAlignment( 290 );
	tmpQLabel->setAlignment( AlignLeft );
	tmpQLabel->setMargin( -1 );
	layout->addWidget (tmpQLabel, rpersdict, 0);
	tmpQLabel->setMinimumWidth (tmpQLabel->sizeHint().width());
	*/


	
	/* for alternate dict

	kle1 = new KLineEdit( this, "LineEdit_1" );
	//	kle1->setGeometry( 150, 120, 290, 30 );
	kle1->setText( "" );
	kle1->setMaxLength( 32767 );
	kle1->setEchoMode( QLineEdit::Normal );
	kle1->setFrame( TRUE );
	connect (kle1, SIGNAL (textChanged (const char*)), this, 
		 SLOT (textChanged1 (const char*)));
	layout->addMultiCellWidget (kle1, rdictedit, rdictedit, 1,4);
	//	kle1->setMinimumSize (290,30);
	*/


	/*
	browsebutton1=new QPushButton;
	browsebutton1 = new QPushButton( this, "PushButton_1" );
	connect( browsebutton1, SIGNAL(clicked()), SLOT(sBrowseDict()) );
	browsebutton1->setText( i18n("Browse...") );
	browsebutton1->setAutoRepeat( FALSE );
	browsebutton1->setAutoResize( FALSE );
	layout->addWidget (browsebutton1, rdictedit, 6);
	browsebutton1->setGeometry( 460, 120, 70, 30 );
	browsebutton1->setMinimumWidth (30);
	*/


	/*
	tmpQPushButton = new QPushButton( this, "PushButton_2" );
	tmpQPushButton->setGeometry( 460, 160, 70, 30 );
	tmpQPushButton->setMinimumWidth(tmpQPushButton->sizeHint().width());
	connect( tmpQPushButton, SIGNAL(clicked()), SLOT(sBrowsePDict()) );
	tmpQPushButton->setText( i18n("Browse...") );
	tmpQPushButton->setAutoRepeat( FALSE );
	tmpQPushButton->setAutoResize( FALSE );
	layout->addWidget (tmpQPushButton, rpersdict, 6);
	//	tmpQPushButton->setMinimumSize (tmpQPushButton->sizeHint());
	*/

	/*
	kle2 = new KLineEdit( this, "LineEdit_2" );
	//	kle2->setGeometry( 150, 160, 290, 30 );
	kle2->setText( "" );
	kle2->setMaxLength( 32767 );
	kle2->setEchoMode( QLineEdit::Normal );
	kle2->setFrame( TRUE );
	connect (kle2, SIGNAL (textChanged (const char*)), this, 
		 SLOT (textChanged2 (const char*)));
	layout->addMultiCellWidget (kle2, rpersdict,rpersdict,1,4);
	///	kle2->setMinimumSize (290,30);
	*/

    }
}

KSpellConfig::~KSpellConfig ()
{
}


bool
KSpellConfig::dictFromList () const
{
  return dictfromlist;
}

bool
KSpellConfig::readGlobalSettings ()
{
  kc->setGroup ("KSpell");

  setNoRootAffix   (kc->readNumEntry ("KSpell_NoRootAffix", 0));
  setRunTogether   (kc->readNumEntry ("KSpell_RunTogether", 0));
  setDictionary    (kc->readEntry ("KSpell_Dictionary", ""));
  setDictFromList  (kc->readNumEntry ("KSpell_DictFromList", FALSE));
  //  setPersonalDict  (kc->readEntry ("KSpell_PersonalDict", ""));
  setEncoding (kc->readNumEntry ("KSpell_Encoding", KS_E_ASCII));
  setClient (kc->readNumEntry ("KSpell_Client", KS_CLIENT_ISPELL));

  return TRUE;
}

bool
KSpellConfig::writeGlobalSettings ()
{
  kc->setGroup ("KSpell");
  kc->writeEntry ("KSpell_NoRootAffix",(int) noRootAffix (), TRUE, TRUE);
  kc->writeEntry ("KSpell_RunTogether", (int) runTogether (), TRUE, TRUE);
  kc->writeEntry ("KSpell_Dictionary", dictionary (), TRUE, TRUE);
  kc->writeEntry ("KSpell_DictFromList",(int) dictFromList(), TRUE, TRUE);
  //  kc->writeEntry ("KSpell_PersonalDict", personalDict (), TRUE,  TRUE);
  kc->writeEntry ("KSpell_Encoding", (int) encoding(),
		  TRUE, TRUE);
  kc->writeEntry ("KSpell_Client", client(),
		  TRUE, TRUE);
  kc->sync();
  return TRUE;
}

void
KSpellConfig::sChangeEncoding(int i)
{
    kDebugInfo( 750, "KSpellConfig::sChangeEncoding(%d)", i);
  setEncoding (i);
}

void
KSpellConfig::sChangeClient (int i)
{
  setClient (i);
}

bool
KSpellConfig::interpret (QString &fname, QString &lname,
			      QString &hname)

{

  //Truncate aff

  if (fname.length()>4)
    if ((signed)fname.find(".aff")==(signed)fname.length()-4)
      fname.remove (fname.length()-4,4);


    kDebugInfo( 750, "KSpellConfig::interpret [%s]", (const char *)fname);

  //These are mostly the ispell-langpack defaults
  if (fname=="english")
    {
      lname="en";
      hname=i18n("English");
    }

  else if (fname=="espa~nol")
    {
      lname="sp";
      hname=i18n("Spanish");
    }

  else if (fname=="deutsch")
    {
      lname="de";
      hname=i18n("German");
    }

  else if (fname=="portuguesb" ||
	   fname=="br")
    {
      lname="br";
      hname=i18n("Brazilian Portuguese");
    }
  
  else
    {
      lname="";
      hname=i18n("Unknown");
    }

  //We have explicitly chosen English as the default here.
  if ( (KGlobal::locale()->language()=="C" && 
	lname=="en") ||
       KGlobal::locale()->language()==lname)
    return TRUE;
    
  return FALSE;
}

void
KSpellConfig::fillInDialog ()
{
  if (nodialog)
    return;

    kDebugInfo( 750, "KSpellConfig::fillinDialog");

  cb1->setChecked (noRootAffix());
  cb2->setChecked (runTogether());
  encodingcombo->setCurrentItem (encoding());
  clientcombo->setCurrentItem (client());

  //  kle1->setText (dictionary());
  //  kle2->setText (personalDict());

  if (langfnames.count()==0) //will only happen once
    {
      
      langfnames.append(""); // Default
      dictcombo->insertItem (i18n("ISpell Default"));

      QFileInfo dir ("/usr/lib/ispell");
      if (!dir.exists() || !dir.isDir())
	dir.setFile ("/usr/local/lib");
      if (!dir.exists() || !dir.isDir())
	return;

            kDebugInfo( 750, "KSpellConfig::fillInDialog %s %s", dir.filePath().ascii(), dir.dirPath().ascii());

      QDir thedir (dir.filePath(),"*.aff");
            kDebugInfo( 750, "KSpellConfig%s\n",thedir.path().ascii());

            kDebugInfo( 750, "entryList().count()=%d", thedir.entryList().count());

      for (unsigned int i=0;i<thedir.entryList().count();i++)
	{
	  QString fname, lname, hname;

	  //	  kdebug (KDEBUG_INFO, 750, "%s/%d %s", __FILE__, __LINE__, (const char *)thedir [i]);
	  fname = thedir [i];

	  if (interpret (fname, lname, hname))
	    { // This one is the KDE default language
	      // so place it first in the lists (overwrite "Default")

	      //	      kdebug (KDEBUG_INFO, 750, "default is [%s][%s]",hname.data(),fname.data());
	      langfnames.remove ( langfnames.begin() );
	      langfnames.prepend ( fname );

	      hname="Default - "+hname+"("+fname+")";
	      
	      dictcombo->changeItem (hname,0);
	    }
	  else
	    {
	      langfnames.append (fname);
	      hname=hname+" ("+fname+")";
	      
	      dictcombo->insertItem (hname.data());
	    }
	}

      
    }
  int whichelement=-1;
  //  kdebug (KDEBUG_INFO, 750, "dfl=%d",dictFromList());
  if (dictFromList())
    for (unsigned int i=0;i<langfnames.count();i++)
      {
	//	kdebug (KDEBUG_INFO, 750, "[%s]==[%s]?", langfnames[i], dictionary().data());
	if (langfnames[i] == dictionary())
	  whichelement=i;
      }

  //  kdebug (KDEBUG_INFO, 750, "whiche=%d", whichelement);
  dictcombo->setMinimumWidth (dictcombo->sizeHint().width());

  if (dictionary().isEmpty() ||  whichelement!=-1)
    {
      setDictFromList (TRUE);
      if (whichelement!=-1)
	dictcombo->setCurrentItem(whichelement);
    }
  else
    setDictFromList (FALSE);

    
  //dictlistbutton->setChecked(dictFromList());
  //  dicteditbutton->setChecked(!dictFromList());
  sDictionary (dictFromList());
  sPathDictionary (!dictFromList());

}

/*
 * Options setting routines.
 */

void
KSpellConfig::setClient (int c)
{
  iclient = c;
}

void
KSpellConfig::setNoRootAffix (bool b)
{
  bnorootaffix=b;
}

void
KSpellConfig::setRunTogether(bool b)
{
  bruntogether=b;
}

void
KSpellConfig::setDictionary (const QString s)
{
  qsdict=s; //.copy();

  if (qsdict.length()>4)
    if ((signed)qsdict.find(".aff")==(signed)qsdict.length()-4)
      qsdict.remove (qsdict.length()-4,4);

  //  kdebug (KDEBUG_INFO, 750, "setdictionary: [%s]",qsdict.data());
}

void
KSpellConfig::setDictFromList (bool dfl)
{
  //  kdebug (KDEBUG_INFO, 750, "sdfl = %d", dfl);
  dictfromlist=dfl;
}

/*
void KSpellConfig::setPersonalDict (const char *s)
{
  qspdict=s;
}
*/

void
KSpellConfig::setEncoding (int enctype)
{
  enc=enctype;
}

/*
  Options reading routines.
 */
int
KSpellConfig::client () const
{
  return iclient;
}


bool
KSpellConfig::noRootAffix () const
{
  return bnorootaffix;
}

bool
KSpellConfig::runTogether() const
{
  return bruntogether;
}

const
QString KSpellConfig::dictionary () const
{
  return qsdict;
}

/*
const QString KSpellConfig::personalDict () const
{
  return qspdict;
}
*/

int
KSpellConfig::encoding () const
{
  return enc;
}

void
KSpellConfig::sRunTogether(bool)
{
  setRunTogether (cb2->isChecked());
}

void
KSpellConfig::sNoAff(bool)
{
  setNoRootAffix (cb1->isChecked());
}

/*
void
KSpellConfig::sBrowseDict()
{
  return;

  QString qs( QFileDialog::getOpenFileName ("/usr/local/lib","*.hash") );
  if ( !qs.isNull() )
    kle1->setText (qs);

}
*/

/*
void KSpellConfig::sBrowsePDict()
{
  //how do I find home directory path??
  QString qs( QFileDialog::getOpenFileName ("",".ispell_*") );
  if ( !qs.isNull() )
      kle2->setText (qs);

  
}
*/

void
KSpellConfig::sSetDictionary (int i)
{
  setDictionary (langfnames[i]);
  setDictFromList (TRUE);
}

void
KSpellConfig::sDictionary(bool on)
{
  if (on)
    {
      dictcombo->setEnabled (TRUE);
      setDictionary (langfnames[dictcombo->currentItem()] );
      setDictFromList (TRUE);
    }
  else
    {
      dictcombo->setEnabled (FALSE);
    }
}

void
KSpellConfig::sPathDictionary(bool on)
{
  return; //enough for now


  if (on)
    {
      //kle1->setEnabled (TRUE);
      //      browsebutton1->setEnabled (TRUE);
      setDictionary (kle1->text());
      setDictFromList (FALSE);
    }
  else
    {
      kle1->setEnabled (FALSE);
      //      browsebutton1->setEnabled (FALSE);
    }
}

void
KSpellConfig::sHelp()
{
  QString file ("kspell/index-2.html"), label ("");
  kapp->invokeHTMLHelp (file, label);
}

/*
void KSpellConfig::textChanged1 (const char *s)
{
  setDictionary (s);
}

void KSpellConfig::textChanged2 (const char *)
{
  //  setPersonalDict (s);
}
*/

void
KSpellConfig::operator= (const KSpellConfig &ksc)
{
  //We want to copy the data members, but not the
  //pointers to the child widgets
  setNoRootAffix (ksc.noRootAffix());
  setRunTogether (ksc.runTogether());
  setDictionary (ksc.dictionary());
  setDictFromList (ksc.dictFromList());
  //  setPersonalDict (ksc.personalDict());
  setEncoding (ksc.encoding());
  setClient (ksc.client());

  fillInDialog();
}

void
KSpellConfig::setIgnoreList (QStringList _ignorelist)
{
  ignorelist=_ignorelist;
}

QStringList
KSpellConfig::ignoreList () const
{
  return ignorelist;
}

#include "ksconfig.moc"



