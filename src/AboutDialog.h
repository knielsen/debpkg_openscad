#ifndef ABOUTDIALOG_H_
#define ABOUTDIALOG_H_

#include "ui_AboutDialog.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

class AboutDialog : public QDialog, public Ui::AboutDialog
{
	Q_OBJECT;
public:
	AboutDialog(QWidget *) {
		setupUi(this);
		this->setWindowTitle( QString("About OpenSCAD ") + QString(TOSTRING( OPENSCAD_VERSION)) );
		this->aboutText->setOpenExternalLinks(true);
		QString tmp = this->aboutText->toHtml();
		tmp.replace("__VERSION__",QString(TOSTRING(OPENSCAD_VERSION)));
		this->aboutText->setHtml(tmp);
	}
};

#endif
