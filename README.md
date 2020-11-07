# The Girl Who Played With The Dragons Nest #

A small roguelike/procedural death labyrinth genre game.

## Installation ##

At the moment, this game is only distributed as source code.  You will need a C++ 17 compiler (I used g++ 9.3.0), the ncurses library and GNU make to compile it.  It should be portable to any system that supports C++ 17 and ncurses but I have only tested it on Linux.

Change to the `release` directory

    $ cd release

or if you want a debug build (only developers will need this) change to the `debug` directory

    $ cd debug

Then from either directory, type:

    $ make

When you are finished, you should have a binary which you can run like this:

    $ ./tgwpwtdn

Optionally you can install the game into `/usr/local/bin` by running as root:

    # make install

The makefile respects PREFIX and DESTDIR if you want to install it elsewhere.

If you want to remove generated files, run:

    $ make clean

To really clean all generated files, change to the top-level directory and run:

    $ make distclean

## How To Play ##

You are in a maze.  Start at the top and  work your way down to the bottom where the dragon dwells.  Slay him and you
win.  Get killed along the way and you lose.

### Items in the maze ###

#### Monsters ####

The maze is filled with monsters which are represented by capital letters.  They have one stamina point so it will
only take one hit to kill them.  They usually do one stamina point of damage on a successful attack except the dragon (D)
who will cost you three stamina points per hit. Another notable monster is the wizard (W.)  On a successful hit,
in addition to losing a stamina point, you will be teleported back to the start.

#### Traps ####

Some sections of the dungeon floor are boobytrapped.  If you get caught in a trap you lose 2 stamina points.  A trap
looks just like an ordinary section of floor but after a trap has been sprung it will be shown as an ^.  It will still 
hurt you if you land on it again.

#### Doors ####

Doors are usually locked.  You can open them if you have a key (shown as a k.)  If you don't, you can batter the door 
down but it will cost you two stamina points.  Closed doors are shown as - or | and  Open doors as /.

#### Potions ####

A potion of healing is shown as !.  Drinking ("quaffing") one will restore your stamina to the initial 10.

#### Armor and Weapons ####

Armor and weapons shown as ] and ) respectively, give you an offensive and/or defensive bonus in fights.  You may arm
yourself ("wield") upto two such items.

### Complete list of Actions ###

These are the direction keys.  They are used for movement or in compound commands to specifiy the direction in which
the action is to occur.

h, left arrow  - move west one space

j, down arrow  - move south one space

k, up arrow    - move north one space

l, right arrow - move east one space

y, home        - move northwest one space

u, page up     - move northeast one space

b, end         - move southwest one space

n, page down   - move southeast one space

Moving onto an item takes it.  Moving onto a monster fights it.

H,J,K,L,Y,U,B,N - run.  Continue moving in the same direction as the lower-case equivalent until a wall, etc. is reached.

m &lt;direction&gt; - move in that direction but don't take what is there.  Can be used to jump over a trap without damage.

M &lt;direction&gt; - run in that direction but don't take anything along the way.

f &lt;direction&gt; - fight whatever is in that direction.

F &lt;direction&gt; - don't stop fighting whatever is in that direction until you or it are dead.

o &lt;direction&gt; - open the door in that direction.  Only works if you have a key.

c &lt;direction&gt; - close the door in that direction.

O &lt;direction&gt; - bash down the door in that direction.

q             - quaff a potion if you have one.

w &lt;number&gt;    - wield an item i.e. move it from inventory slot &lt;number&gt; into your hand ready for use.

U &lt;number&gt;    - put wielded item &lt;number&gt; back into your inventory.

d &lt;number&gt;    - drop wielded or carried item &lt;number&gt;.

,             - take whatever is in the square you are standing on.  Handy if you used m previously.

v             - display version info.

!             - temporarily drop to a command shell.  type exit to return to the game.

Ctrl-R        - refresh the screen if has gotten messed up.

Q             - quit the program.

## Author and Copyright ##

Jaldhar H. Vyas <jaldhar@braincells.com>  
Copyright (C) 2014, Consolidated Braincells Inc. All rights reserved.
This program may be distributed in source or compiled form under the terms of the BSD license.  

