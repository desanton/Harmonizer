// https://github.com/Tonejs/Tone.js/tree/master 
// Tone.sampler section
const sampler = new Tone.Sampler({
    urls: {
        "A7": "A7.mp3",
        "A1": "A1.mp3",
        "A2": "A2.mp3",
        "A3": "A3.mp3",
        "A4": "A4.mp3",
        "A5": "A5.mp3",
        "A6": "A6.mp3",
        "A#7": "As7.mp3",
        "A#1": "As1.mp3",
        "A#2": "As2.mp3",
        "A#3": "As3.mp3",
        "A#4": "As4.mp3",
        "A#5": "As5.mp3",
        "A#6": "As6.mp3",
        "B7": "B7.mp3",
        "B1": "B1.mp3",
        "B2": "B2.mp3",
        "B3": "B3.mp3",
        "B4": "B4.mp3",
        "B5": "B5.mp3",
        "B6": "B6.mp3",
        "C7": "C7.mp3",
        "C1": "C1.mp3",
        "C2": "C2.mp3",
        "C3": "C3.mp3",
        "C4": "C4.mp3",
        "C5": "C5.mp3",
        "C6": "C6.mp3",
        "C7": "C7.mp3",
        "C#7": "Cs7.mp3",
        "C#1": "Cs1.mp3",
        "C#2": "Cs2.mp3",
        "C#3": "Cs3.mp3",
        "C#4": "Cs4.mp3",
        "C#5": "Cs5.mp3",
        "C#6": "Cs6.mp3",
        "D7": "D7.mp3",
        "D1": "D1.mp3",
        "D2": "D2.mp3",
        "D3": "D3.mp3",
        "D4": "D4.mp3",
        "D5": "D5.mp3",
        "D6": "D6.mp3",
        "D#7": "Ds7.mp3",
        "D#1": "Ds1.mp3",
        "D#2": "Ds2.mp3",
        "D#3": "Ds3.mp3",
        "D#4": "Ds4.mp3",
        "D#5": "Ds5.mp3",
        "D#6": "Ds6.mp3",
        "E7": "E7.mp3",
        "E1": "E1.mp3",
        "E2": "E2.mp3",
        "E3": "E3.mp3",
        "E4": "E4.mp3",
        "E5": "E5.mp3",
        "E6": "E6.mp3",
        "F7": "F7.mp3",
        "F1": "F1.mp3",
        "F2": "F2.mp3",
        "F3": "F3.mp3",
        "F4": "F4.mp3",
        "F5": "F5.mp3",
        "F6": "F6.mp3",
        "F#7": "Fs7.mp3",
        "F#1": "Fs1.mp3",
        "F#2": "Fs2.mp3",
        "F#3": "Fs3.mp3",
        "F#4": "Fs4.mp3",
        "F#5": "Fs5.mp3",
        "F#6": "Fs6.mp3",
        "G7": "G7.mp3",
        "G1": "G1.mp3",
        "G2": "G2.mp3",
        "G3": "G3.mp3",
        "G4": "G4.mp3",
        "G5": "G5.mp3",
        "G6": "G6.mp3",
        "G#7": "Gs7.mp3",
        "G#1": "Gs1.mp3",
        "G#2": "Gs2.mp3",
        "G#3": "Gs3.mp3",
        "G#4": "Gs4.mp3",
        "G#5": "Gs5.mp3",
        "G#6": "Gs6.mp3",
    },
    release: 1,
    baseUrl: "pianoSamples/"
}).toDestination();

// https://www.devbridge.com/articles/tonejs-coding-music-production-guide/

Tone.Transport.bpm.value = 150


// https://github.com/Tonejs/Tone.js/wiki/Time
// Tempo and time signature relative time in the form BARS:QUARTERS:SIXTEENTHS.
//   "32:0:0" = start of the 32nd measure.
//   "4:3:2" = 4 bars + 3 quarter notes + 2 sixteenth notes.
//   "1:2" = 1 bar + 2 quarter notes (sixteenth notes can be omitted)
// Notation Describes time in BPM and time signature relative values.
//   "4n" = quarter note
//   "8t" = eighth note triplet
//   "2m" = two measures
//   "8n." = dotted-eighth note

// https://tonejs.github.io/docs/r11/Part for note velocity (volume)
const chordVolume = 0.3;
