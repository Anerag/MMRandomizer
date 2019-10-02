
Download windows binary: https://laatikko.io/mmrandomizer/index.html

Might and Magic Randomizer
-----------------------------

Version 1.4

Note: if you get an error like "vcruntime 140_1.dll missing" or so, you will
need to download and install the Visual C++ 2019 redistributable.
https://aka.ms/vs/16/release/vc_redist.x64.exe

You will need MMArchive to modify the game files, get it here:
https://grayface.github.io/mm/#MMArchive

Next find where your game is installed, example:
C:\Program Files (x86)\Might and Magic VI\

If you want to play using GrayFace's patch, be sure to uncheck the "install
LOD archives" option in the patch installer. If you've already patched the
game, you can go to "<path to game>\data" and delete or move all ".lod" files
named "<number> patch.<something>.lod".

If you really want to use the patched ".lod"s then it's up to you to keep track
of which file is from which ".lod".


How to use

1. Backup
Make a copy of "<path to game>\data". When you're done playing with the
randomizer, you can restore the unmodified files. Otherwise you will need to
reinstall the game!

2. Extract data files
Open "data\games.lod" with MMArchive. Now select ".dlv" in the dropdown menu
on the toolbar, then drag and drop all files into the data folder where the
randomizer is (mm6data for MM6 files for example). Do this for ".ddm" as well.

The ".evt" files are game specific.

MM6: Open "data\icons.lod" and do the same for ".evt".
MM7: Open "data\events.lod" and do the same for ".evt".

MM6 only: if you want to randomize monsters, extract "mapstats.txt" from
"icons.lod".

GrayFace's patched ".lod"s only: I hope you know what you're doing.


3. Randomize
Run MMRandomizer.exe, it will modify the files in the data folders only. It
generates a text file named "<game>-randomizer-<random seed>.txt". This file
shows you the locations for all the randomized items which you can refer to in
case you happen to get stuck.

Note: if you get any "couldn't open file: <file>" errors when running the
randomizer, make sure you extracted those files and placed them in the correct
data folder.

4. Repack data files
Drag and drop all ".dlv", ".ddm", ".evt" and ".txt" back into their respective
".lod". Sorting files by type makes this easier.

5. Play the game
You're done! 


Configuration

The "<game>itemdata.txt" files list all the items the randomizer can modify.
Note that there are seperate files for GrayFace's patched ".lod"s.

Items on lines beginning with '+' will be randomized.
Items on lines beginning with '-' will be at their normal locations.

You may change these however you like, just keep in mind some items might
end up in unreachable locations depending on the configuration.


Changelog

1.4 - Better GrayFace patch support
1.3 - MM7 event fixes, MM6 more items
1.2 - MM6 monsters
1.1 - MM7 items
1.0 - MM6 items
