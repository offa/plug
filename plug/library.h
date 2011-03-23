#ifndef LIBRARY_H
#define LIBRARY_H

#include <QDialog>
#include <QFileDialog>
#include <QDir>
#include <QResizeEvent>

namespace Ui {
    class Library;
}

class Library : public QDialog
{
    Q_OBJECT

public:
    explicit Library(char names[24][32], QWidget *parent = 0);
    ~Library();

private:
    Ui::Library *ui;
    QFileInfoList *files;
    void resizeEvent(QResizeEvent *);

private slots:
    void load_slot(int);
    void get_directory();
    void get_files(QString);
    void load_file(int);
    void change_font_size(int);
    void change_font_family(QFont);

signals:
    void directory_changed(QString);
};

#endif // LIBRARY_H
