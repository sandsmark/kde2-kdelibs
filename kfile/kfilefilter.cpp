
#include "kfilefilter.h"
#include <qstrlist.h>
#include <kapp.h>
#include <klocale.h>
KFileFilter::KFileFilter( QWidget *parent, const char *name)
    : QComboBox(true, parent, name), filters(0)
{
    setInsertionPolicy(NoInsertion);
    connect(this, SIGNAL(activated(const QString &)), SLOT(changed(const QString &)));
}

KFileFilter::~KFileFilter()
{
    delete filters;
}

void KFileFilter::changed( const QString & )
{
    emit filterChanged();
}

void KFileFilter::setFilter(const char *filter)
{
    delete filters;
    filters = new QStrList( true );
    if (filter) {
	char *tmp = qstrdup(filter); // deep copy
	char *g = strtok(tmp, "\n");
	while (g) {
	    filters->append(g);
	    g = strtok(0, "\n");
	}
	delete [] tmp;
    } else
	filters->append(i18n("*|All Files").ascii());

    clear();
    QString name;
    for (const char *item = filters->first(); item; 
	 item = filters->next()) {
	name = item;
	int tab = name.find('|');
	insertItem((tab < 0) ? name :
		   name.mid(tab + 1, name.length() - tab));
    }
}

QString KFileFilter::currentFilter() 
{
    QString filter = currentText();
    if (filter == text(currentItem()))
	filter = filters->at(currentItem());
    
    int tab = filter.find('|');
    if (tab < 0)
	return filter;
    else
	return filter.left(tab);
}

#include "kfilefilter.moc"

