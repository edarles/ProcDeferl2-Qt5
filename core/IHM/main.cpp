#include <window.h>
#include <qapplication.h>

int main(int argc,char **argv)
{
  printf("\nProcDeferl, Copyrigth (c) 2015 M. Parenthoen, E. Darles\n\n");
  //glutInit(&argc, argv);
  //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  setenv ("LC_NUMERIC","POSIX",1);
  QApplication app(argc,argv);
  Window *window = new Window();
  window->show();
  return app.exec();
}

