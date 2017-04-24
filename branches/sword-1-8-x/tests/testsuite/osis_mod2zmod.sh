#!/bin/sh

rm -rf tmp/osis_mod2zmod/
mkdir -p tmp/osis_mod2zmod/mods.d
mkdir -p tmp/osis_mod2zmod/modules
mkdir -p tmp/osis_mod2zmod/zmodules

cat > tmp/osis_mod2zmod/mods.d/osisreference.conf <<!
[OSISReference]
DataPath=./modules/
ModDrv=RawText
#ModDrv=zText
#CipherKey=abc123
Encoding=UTF-8
BlockType=BOOK
CompressType=ZIP
SourceType=OSIS
Lang=en
GlobalOptionFilter=OSISStrongs
GlobalOptionFilter=OSISMorph
GlobalOptionFilter=OSISFootnotes
GlobalOptionFilter=OSISHeadings
GlobalOptionFilter=OSISRedLetterWords
Feature=StrongsNumbers
!

cat > tmp/osis_mod2zmod/mods.d/zosisreference.conf <<!
[zOSISReference]
DataPath=./zmodules/
#ModDrv=RawText
ModDrv=zText
CipherKey=abc123
Encoding=UTF-8
BlockType=BOOK
CompressType=ZIP
SourceType=OSIS
Lang=en
GlobalOptionFilter=OSISStrongs
GlobalOptionFilter=OSISMorph
GlobalOptionFilter=OSISFootnotes
GlobalOptionFilter=OSISHeadings
GlobalOptionFilter=OSISRedLetterWords
Feature=StrongsNumbers
!

../../utilities/osis2mod tmp/osis_mod2zmod/modules/ osisReference.xml 2>&1 | grep -v \$Rev

cp osis_basic.good osis_mod2zmod.good
cd tmp/osis_mod2zmod
../../../../utilities/mod2zmod OSISReference zmodules/ 4 2 0 abc123 > /dev/null 2>&1
../../../osistest zOSISReference
