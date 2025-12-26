Day 0 15-12-2025, workload 6 hours 
====

Plan
----

* We need to at least be able to add and display bases, with the right colour and maybe with a flag, Done

* Add option to select base by right-clicking (add highlight to base), Done

* Start working on the architecture of the UI, Done

* Load a selection of countries in Europe and North Africa, and the middle east, Done

* Try to add: uiTexture, uiTextline, uiLineList, maybe add uiButton, Done

Work
----
I am able to display bases, yet I am missing a way of placing them down, I have only loaded Denmark, Norway and Sweden

The UI system is working beautifully, I have both text, textures listLines and buttons

Issues
-----
If the program throws an exception while loading, some textures  (like ball in water, base, uiBackground, or the buttons) are not properly disposed off

day 1 16-12-2025, workload 6 hours
===============

plan
----
* Get the program working on Windows OS, Done, I also had to fixe the texture unloading issue

* Add the option to create bases with shift+click, Done

* Select bases with right-click and open up another menu (goes into edit base mode). Done

* Edit base income, ownership, and have option to delete base. Done

* Have option to name bases and provinces, that might be difficult, Done, that was easy.

* Consider how to get cores to work, maybe there should only be one core owner. Done, but didn't add option to change it

* Add a bunch of new countries and automatic country loading. Failed

work
---
I had some time to work, because I was waiting for a meeting call which, of course, never came.

I can now add bases, and edit some aspects of the bases.

day 2 17-12-2025, workload 9 hours, mainly adding countries
=============

plan
----
* Make base icon bigger, Done, (but I liked the smaller icons better)

* Add option to change core and owner from edit menu Done (but multiple cores are clearly needed Romania should core Moldova) (correction, single cores are more realistic)

* Add ability to view longitude/latitude of mouse. Done

* Add all countries and automatic country loading: Failed, I added most, but I am still missing 15 countries, and didn't get to loading


Work
----
I mainly added countries, the entire day, there are a lot of countries in Europe, seriously a lot

Issues
-----
Political issue: the game needs to support multiple CORE owners: example, Both Libyan governments need to core all of Libya

Correction, NO, I think single cores is more realistic. Instead there need to be unique events to gain cores

day 2.5 18-12-2025, workload 4 hours (half day)
=============

plan
----
* Make base icons smaller again, Done.

* Add the remaining countries, and automatic loading (ALMOST done)

* Add option to delete base (Failed)

* Add ability to save and load (Failed)

Work
----
I added almost all countries, and the option to load.
Loading is problematic due to SDL texture threading issues, I had a lot of crash bugs before I settled on sub-optimal single thread loading.

Issues
------
I need to multithread country loading, multithreading SDL texture loading is not possible, I need to seperate loading from finalization

day 3.5 19-12-2025, workload 9 hours
=============

plan
----
* Add the remaining countries (Done)

* Add ability to set neighbours (Done)

* Add ability to save and load (Done)

* Add display of core as well as owner (Failed, I completely forgot)

* Get a better font and fix issues rendering  (Done, Atkinson Hyperlegible must be the best)

* Start thinking about which cities to add (Oh my god, there are a lot of cities in Europe)

Work
----
Oh my god, there are a lot of cities in europe, like really a lot, I thought using 100000 population as a cut-of was a good idea, but there are hundreds of cities above that population.

I think there is nothing for it, I will just have to spend forever adding cities ... well, that is what I created this program for

Oh, I also got loading and saving working, and added the option to declare neighbours ... but I am going to only do that later, right now, I have hundreds of cities to add.

On a further thought, I won't commit to the 100000 limit everywhere, I will adjust it up or down for densely or sparsely populated regions

Issues
------
I need to multithread country loading, multithreading SDL texture loading is not possible, I need to separate loading from finalization

Fix Turkey missing ball

Fix France wrong colour

Only display city textures when on screen

day 4.5 20-12-2025, workload 9 hours
=============

Summary
-----
I forgot this log on the day, I basically spent the entire day adding cities in the Maghreb and middle east.

day 5.5 21-12-2025,  Workload 11 hours
=======

plan
-----
Add the bases for the following very ambitious list of countries: DONE, that took like forever
* France (Done)
* Germany (Done)
* Belgium (Done)
* Netherlands (Done)
* Luxembourg (that one should be fast) (Done)
* Italy (Done)
* Austria (Done)
* Slovenia (Done)
* Czechia (Done)
* Slovakia (Done)
* Hungary (Done)
* Croatia (Done)
* Serbia (Done)
* Bosnia and Herzegovina (Done)
* Montenegro (Done)
* Kosovo (Done)
* Albania (Done)
* North Macedonia (Done)

Fix missing countryball for Türkiye (Done)

Fix wrong colour for Bosnia and hzg., kosovo, austria, serbia (should be white or dark blue) (Done)

Add flags to the bases themself (Done, I don't know if it is better)

Work
----
I had a lot of time to work, now when the holiday has begun, so I worked a lot

Issues
------
I need to multithread country loading, multithreading SDL texture loading is not possible, I need to separate loading from finalization

Only display city textures when on screen

day 6.5 22-12-2025,  Workload 12 hours
=======

Plan
----

Add the flag and countryball for andorra

Fix Colour of Finland, and maybe norway or Estonia

Add the bases for the following very ambitious list of countries:  DONE
* Poland Done
* Lithuania Done
* Latvia Done
* Estonia Done
* Romania Done
* Bulgaria Done
* Ukraine Done
* Turkey Done
* Iraq Done
* Georgia Done
* Norway Done
* Sweden Done
* Finland Done
* Andorra Done
* Belarus Done
* Russia DONE DONE DONE DONE DONE ... DONE

Work
-----
I added cities all day, it was terrible, but it is done, I am actually truly fully done with adding cities.

Issues
------
I need to multithread country loading, multithreading SDL texture loading is not possible, I need to separate loading from finalization

Only display city textures when on screen

day 7.5 23-12-2025, workload 8 hours
=======

Plan
----
Rename bases to cities (Done)

Allow zooming further in, without scaling up the city texture (Done)

record a video on: the project, the progress (Done and uploaded)

Prepare thorough plan for the game (for the video) (Done)

Start working on neighbours and maybe implement borders (Done I guess, at least I tried, and failed)

work
-----
I recorded and uploaded a video on my progress, that took most of the time.

I also did a lot of thinking about how to handle the front-lines, and I got something working and it is worthless garbage which need to be completely replaced

But well, I didn't know that it would be bad before I tried it, so the work still counts

oh, and I went through he map and added all neighbours

day 8 24-12-2025, workload 4 hours
=======

Note
----
I forgot to write the devlog for this day, I worked on the main menu for the finished game because I felt like I needed to do something else

day 9 25-12-2025, workload 12 hours
=======

Note
----
I forgot to write the devlog for this day, I worked on the main menu for the finished game because I felt like I needed to do something else

In particular I worked on the select country menu, which involved writing a lot of description texts for each country.

day 10 26-12-2025
=====

plan
----
Finish writing descriptions for every country. Done

Display all relevant information for selected countries. Done

Open up the game, load tiles asynchronously and display the map FAILED, AND HUGE ISSUE FOUND

Work
----
While working on the asynchronous loading of the tiles, I had an unexplainable undignified crash (where the program crashed without throwing an exception)

This led me down a rabit-hole, which culminated with me realising that `IMG_Load` is not thread safe!!!

If the three exclamation marks doesn't give it away, this is a huge issue, because I have used multithreaded image loading quite liberally
I need to completely revamp the texture loading infrastructure, but at least I know what is wrong, and what needs to be done

issues
--------
IMG_Load is not thread safe, replace it with another png loading function