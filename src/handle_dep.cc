#include "handle_dep.h"
#include "myqhash.h"
#include <string>
#include <sstream>
#include <QString>
#include <QDir>
#include <QSet>
#include <stdlib.h> // for system()

QSet<std::string> dependencies;
const char *make_command = NULL;

void handle_dep(const std::string &filename)
{
	if (filename[0] == '/')
		dependencies.insert(filename);
	else {
		QString dep = QDir::currentPath() + QString("/") + QString::fromStdString(filename);
		dependencies.insert(dep.toStdString());
	}
	if (!QFile(QString::fromStdString(filename)).exists() && make_command) {
		std::stringstream buf;
		buf << make_command << " '" << QString::fromStdString(filename).replace("'", "'\\''").toUtf8().data() << "'";
		system(buf.str().c_str()); // FIXME: Handle error
	}
}

bool write_deps(const std::string &filename, const std::string &output_file)
{
	FILE *fp = fopen(filename.c_str(), "wt");
	if (!fp) {
		fprintf(stderr, "Can't open dependencies file `%s' for writing!\n", filename.c_str());
		return false;
	}
	fprintf(fp, "%s:", output_file.c_str());
	QSetIterator<std::string> i(dependencies);
	while (i.hasNext())
		fprintf(fp, " \\\n\t%s", i.next().c_str());
	fprintf(fp, "\n");
	fclose(fp);
	return true;
}
