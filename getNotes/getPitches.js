const fs = require('fs');
const esLib = require('essentia.js');
const essentia = new esLib.Essentia(esLib.EssentiaWASM);

let wav = require('node-wav');
let buffer = fs.readFileSync('../inputMusic/input.wav');
let adio = wav.decode(buffer);

if(adio.channelData.length == 2) {
    //dunno if this works
    
    // // Convert audio data into VectorFloat type
    // const audioLeftChannelData = essentia.arrayToVector(adio.channelData[0]);
    // const audioRightChannelData = essentia.arrayToVector(adio.channelData[1]);
    // // Downmix stereo audio to mono
    // adio = essentia.MonoMixer(audioLeftChannelData, audioRightChannelData);
}
else if (adio.channelData.length >= 2) {
    throw new Error("bruh more than 2 audio channels");
}


//console.log(adio.channelData[0]);
const samplingRate = 44100 // From "audio midi setup" app, default macbook pro microphone sampling rate = 48000, not 44100
const audioVector = essentia.arrayToVector(adio.channelData[0]);
const audioArray = essentia.vectorToArray(audioVector);
const duration = audioArray.length / samplingRate;
console.log("audioArray.length: "); console.log(audioArray.length);
console.log("duration: "); console.log(duration);

let pitchMelodia = essentia.PitchMelodia(audioVector, //, input parameters (optional)
                                         10, // binResolution,
                                         3, // filterIterations
                                         2048, // frameSize
                                         false, // guessUnvoiced
                                         0.8, // harmonicWeight
                                         128, // hopSize
                                        ); 
const keyData = essentia.KeyExtractor(audioVector,
                                      true, // AverageDetuningCorrection (default=true):     shifts a pcp to the nearest tempered bin
                                      4096, // frameSize (integer ∈ (0, ∞), default = 4096): the framesize for computing tonal features
                                      4096, // hopSize (integer ∈ (0, ∞), default = 4096):   the hopsize for computing tonal features
                                      12,   // hpcpSize (integer ∈ [12, ∞), default = 12):   the size of the output HPCP (must be a positive nonzero multiple of 12)
                                      3500, // maxFrequency (real ∈ (0, ∞), default = 3500): max frequency to apply whitening to [Hz]
                                      60,   // maximumSpectralPeaks (integer ∈ (0, ∞), default = 60): the maximum number of spectral peaks
                                      25,   // minFrequency (real ∈ (0, ∞), default = 25):   min frequency to apply whitening to [Hz]
                                      0.2,  // pcpThreshold (real ∈ [0, 1], default = 0.2):  pcp bins below this value are set to 0
                                      'bgate', // profileType (string ∈
                                               // {diatonic, krumhansl, temperley, weichai, tonictriad, temperley2005, thpcp, shaath, gomez, noland, faraldo,
                                               // pentatonic, edmm, edma, bgate, braw}, default = bgate): the type of polyphic profile to use for correlation calculation
                                      samplingRate, // sampleRate (real ∈ (0, ∞), default = 44100): the sampling rate of the audio signal [Hz]
                                      0.0001,// spectralPeaksThreshold (real ∈ (0, ∞), default = 0.0001): the threshold for the spectral peaks
                                      440,   // tuningFrequency (real ∈ (0, ∞), default = 440): the tuning frequency of the input signal
                                      'cosine',// weightType (string ∈ {none, cosine, squaredCosine}, default = cosine):
                                               // type of weighting function for determining frequency contribution
                                      'hann'); // windowType (string ∈ {hamming, hann, hannnsgcq, triangular, square, blackmanharris62,
                                               // blackmanharris70, blackmanharris74, blackmanharris92}, default = hann): the window type
const bpm = essentia.PercivalBpmEstimator(audioVector, 1024, 2048, 128, 128, 210, 50, samplingRate).bpm;

console.log("Key: "); console.log(keyData)
console.log("BPM: "); console.log(bpm)

const pitches = essentia.vectorToArray(pitchMelodia.pitch);
const pitchesConfidence = essentia.vectorToArray(pitchMelodia.pitchConfidence);
const N = pitches.length;
console.log("pitches.length: "); console.log(pitches.length);
// N = audioArray.length / hopSize = audioArray.length / 128
//pitches.forEach(item => console.log(item))
//pitchesConfidence.forEach(item => console.log(item))

var S = "" + duration.toString() + " " + keyData.key.toString() + " " + keyData.scale.toString() + " " +  bpm.toString() + " " +  N.toString();

for(var i = 0; i < N; i++) {
    S += " " + pitches[i].toString();
}


fs.writeFile("../blobs/pitches.txt", S, function(err) {
        if(err) {
            return console.log(err);
        }
        console.log("The file was saved!");
});




