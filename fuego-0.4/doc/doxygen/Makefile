# Projects in order of dependencies (tag files for lower level projects must
# be generated before the documentation of the projects that depend on them)
PROJECTS = \
  general \
  gtpengine \
  smartgame \
  go \
  simpleplayers \
  gouct \
  fuegotest \
  fuegomain

PAGES = \
  index.html \
  main.html \
  toc.html

.PHONY: changelog.txt copy doxygen clean

all: doxygen

doxygen:
	mkdir -p fuego-doc
	@for dir in $(PROJECTS); do \
            if test -f $$dir/Makefile; then \
                make -C $$dir || exit -1; \
            fi; \
        done
	(DATE=`date`; \
        for p in $(PAGES); do \
            sed "s/\$$datetime/$$DATE/" <pages/$$p >fuego-doc/$$p; done)
	cp pages/go.jpg pages/doxygen.css fuego-doc

clean:
	@for dir in $(PROJECTS); do \
            if test -f $$dir/Makefile; then \
                echo -e "\n--------------- Removing temporary files for $$dir"; \
                make -C $$dir clean; \
            fi; \
        done
	-rm -rf fuego-doc
