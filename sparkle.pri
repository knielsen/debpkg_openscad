sparkle {
  OPENSCAD_LIBRARIES_DIR = $$(OPENSCAD_LIBRARIES)
  !isEmpty(OPENSCAD_LIBRARIES_DIR) {
    QMAKE_OBJECTIVE_CFLAGS +=-F$$OPENSCAD_LIBRARIES_DIR/lib
    QMAKE_LFLAGS +=-F$$OPENSCAD_LIBRARIES_DIR/lib
  }

  LIBS += -framework Sparkle
  HEADERS += src/SparkleAutoUpdater.h
  OBJECTIVE_SOURCES += src/SparkleAutoUpdater.mm
}
