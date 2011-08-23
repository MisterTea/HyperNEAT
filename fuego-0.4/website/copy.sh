FILES="\
favicon.ico \
fuego-screenshot.html \
fuego-screenshot.jpg \
fuego-screenshot-thumb.jpg \
index.html \
"

echo "Enter user id for SourceForge:"
read NAME

scp $FILES $NAME,fuego@web.sourceforge.net:/home/groups/f/fu/fuego/htdocs
