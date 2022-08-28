Harmonizer is a prototype system to generate a chord progression (harmony) for a given melody.

Video showcasing the results (timestamps in the description of the video): https://youtu.be/N9ExizulhUw

To run the Harmonizer prototype locally, follow the following 5 steps.
Note that this prototype has only been tested on MacOS.

1. Install Node.js 

         Download from here: https://nodejs.org/en/download/

2. Initialize Node.js in Harmonizer directory

         Run command:
         ...../harmonizer $ npm init

         This command should create a folder named "node_modules" in your directory.
         It contains all necessary dependencies, such as "tone.js" and "essentia.js" already installed.
         This is due to the package-lock.json file.

3. If applicable, save musicxml file (see harmonizer.pdf paper)

         If you want to play your own melody as opposed to the existing melodies, you can
         add a musicxml file to the inputMusic folder.

         musicxml files can be exported from many music-playing apps (see harmonizer.pdf paper).

4. Install firefox

         OR search for the word "firefox" in harmonizer.cpp
         and replace it with the browser app of your choice

5. Run createHarmony/harmonizer.cpp

         In harmonizer/createHarmony, compile C++ file:
         ...../harmonizer/createHarmony $ g++ harmonizer.cpp -o harmonizer 
         Alternatively, use the Makefile:
         ...../harmonizer/createHarmony $ make clean; make

         Run the executable with command line arguments.

         Command line argument examples:
         ...../harmonizer/createHarmony $ ./harmonizer 3 melodyHBf.js
         ...../harmonizer/createHarmony $ ./harmonizer 3 melodyFEam.js
         ...../harmonizer/createHarmony $ ./harmonizer 5 melodyHBf.musicxml major
         ...../harmonizer/createHarmony $ ./harmonizer 5 melodyHBf.musicxml minor

         Note that the arguments 3 and 5 are options which specify these operations:
         option = 3 => use playNotes/filename as .js input
                  5 => use inputMusic/filename as .musicxml input 

         For options = 5, you must also specify the whether the melody is major or minor.

         Running harmonizer will open a local host in your browser and play back
         all the harmonizations of your melody.

         The harmonizations will be played in the order found in "sample-output.txt".

         To see all supported command-line arguments, run the executable without any arguments:
         ...../harmonizer/createHarmony $ ./harmonizer

         Usage: ./harmonizer option filename major/minor
         mode = 0 => use microphone input
                1 => use inputMusic/filename as .wav input
                2 => use inputMusic/input.wav as input
                3 => use playNotes/filename as .js input
                4 => use playNotes/filename as .js input. Generate files but skip audio output 
                5 => use inputMusic/filename as .musicxml input 
         Examples:
         ./harmonizer 0
         ./harmonizer 1 fMajScale.wav
         ./harmonizer 2
         ./harmonizer 3 melodyHBf.js
         ./harmonizer 4 melodyHBf.js
         ./harmonizer 5 melodyHBf.musicxml major
         ./harmonizer 5 melodyHBf.musicxml minor

         As explained in the harmonizer.pdf paper, option = 3 and option = 5 work perfectly. 
         Options 0, 1, 2 work well but are not yet perfect. These options are a work in progress. 
