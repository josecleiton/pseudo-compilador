QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle
LIBS += -lstdc++fs

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
  src/ast.cpp \
  src/erro.cpp \
  src/lex.cpp \
  src/main.cpp \
  src/sem.cpp \
  src/syn.cpp \
  src/stable.cpp \
  src/token.cpp \
  src/global.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
  src/ast.hpp \
  src/erro.hpp \
  src/lex.hpp \
  src/sem.hpp \
  src/syn.hpp \
  src/stable.hpp \
  src/token.hpp \
  src/global.hpp

DISTFILES += \
  src/Makefile \
  test/test.c20192 \
  test/valido.c20192
