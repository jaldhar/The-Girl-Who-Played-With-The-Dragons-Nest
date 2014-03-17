The-Girl-Who-Played-With-The-Dragons-Nest
=========================================

A small roguelike/procedural death labyrinth genre game.

Installation
============

You will need a c++ 11 compiler (I used g++ 4.6.3), the ncurses library and qmake from the Qt toolkit.  Run qmake to
create the Makefile and then run make to compile the program.  It should be portable to any system that supports c++ 11
and ncurses but I have only tested it on Linux.

I. How To Play
==============

You are in a maze.  Start at the top and  work your way down to the bottom where the dragon dwells.  Slay him and you
win.  Get killed along the way and you lose.

II.  Items in the maze.
======================

Monsters
========

The maze is filled with monsters which are represented by capital letters.  They have one stamina point so it will
only take one hit to kill them.  They usually do one stamina point of damage on a successful attack except the dragon (D)
who will cost you three stamina points per hit. Another notable monster is the wizard (W.)  On a successful hit,
in addition to losing a stamina point, you will be teleported back to the start.


Traps
=====

Some sections of the dungeon floor are boobytrapped.  If you get caught in a trap you lose 2 stamina points.  A trap
looks just like an ordinary section of floor but after a trap has been sprung it will be shown as an ^.  It will still 
hurt you if you land on it again.


Doors
=====

Doors are usually locked.  You can open them if you have a key (shown as a k.)  If you don't, you can batter the door 
down but it will cost you two stamina points.  Closed doors are shown as - or | and  Open doors as /.


Potions
=======

A potion of healing is shown as !.  Drinking ("quaffing") one will restore your stamina to the initial 10.


Armor and Weapons
=================

Armor and weapons shown as ] and ) respectively, give you an offensive and/or defensive bonus in fights.  You may arm
yourself ("wield") upto two such items.


III. Complete list of Actions
=============================

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

m <direction> - move in that direction but don't take what is there.  Can be used to jump over a trap without damage.
M <direction> - run in that direction but don't take anything along the way.
f <direction> - fight whatever is in that direction.
F <direction> - don't stop fighting whatever is in that direction until you or it are dead.
o <direction> - open the door in that direction.  Only works if you have a key.
c <direction> - close the door in that direction.
O <direction> - bash down the door in that direction.
q             - quaff a potion if you have one.
w <number>    - wield an item i.e. move it from inventory slot <number> into your hand ready for use.                                                         
u <number>    - put wielded item <number> back into your inventory.
d <number>    - drop wielded or carried item <number>.
,             - take whatever is in the square you are standing on.  Handy if you used m previously.
v             - display version info.
!             - temporarily drop to a command shell.  type exit to return to the game.
Ctrl-R        - refresh the screen if has gotten messed up.
Q             - quit the program.

IV. Author and Copyright
========================

Jaldhar H. Vyas <jaldhar@braincells.com>
Copyright (C) 2014, Consolidated Braincells Inc. All rights reserved.
This program may be distributed in source or compiled form under the terms of the BSD license.

