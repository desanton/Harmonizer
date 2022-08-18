#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <cassert>
#include <math.h>  
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <ctime>
#include <array>

using namespace std;

//STRUCTS
struct Note
{
    private:
        int MIDI = -1;
        float relativeNote = -1;
        float emphasis = -1;
    public: 
        bool mainTone = true;
        float time;
        //note is the tone + octave
        //ex: "A5"
        string note;
        float duration;

        float getEmphasis(float totalNoteDuration, float numberOfNotes) {
            if(emphasis != -1) return emphasis;
            
            float avgNoteDuration = numberOfNotes/totalNoteDuration;

            float relativeTime = duration/totalNoteDuration;

            emphasis = relativeTime/avgNoteDuration;

            return emphasis;
        }

        int getOctave() {
            assert(note.size() > 0 && "empty note error");

            return note[note.size() - 1] - '0';
        }

        float getRelativeNote(vector<string> notesOfScale) {
            //If relative note already calculated
            if (relativeNote != -1) return relativeNote;
            
            string noteWithoutOctave = note.substr(0, note.length() - 1);
            
            for(float i = 1; i <= 7; i++) {
                if(notesOfScale[(int) i - 1] == noteWithoutOctave) {
                   relativeNote = i;

                   return relativeNote;
                }
            }

            mainTone = false;

            //for now, we disregard accidentals as passing tones no matter what
            //TODO: decide when accidentals are main tones
            return (float) -1;
        }
};

struct Chord
{
    private:

    public:
        float time;
        string chord;
        float duration;

        //chord can either be a simple chord name (ex: "IChord")
        //or a vector of notes (ex: ["A5", "C#5", "E5"])
        vector<string> notesOfChord;

        //always assume the first note is the lowest, the next is higher, and so on
        //ex: notesOfChord = ["A", "C#", "E", "A"] 
        //addOctaves(3) --> notesOfChord = ["A3", "C#4", "E4", "A4"]
        void addOctaves(int lowestNoteOctave) {
            if(notesOfChord.size() <= 0) return;
            
            int oct = lowestNoteOctave;

            map<string, int> notes; 
            notes["C"] = 1;
            notes["C#"] = 2;
            notes["D"] = 3;
            notes["D#"] = 4;
            notes["E"] = 5;
            notes["F"] = 6;
            notes["F#"] = 7;
            notes["G"] = 8;
            notes["G#"] = 9;
            notes["A"] = 10;
            notes["A#"] = 11;
            notes["B"] = 12;

            string prevNote = notesOfChord[0];

            notesOfChord[0] = notesOfChord[0] + to_string(oct);

            for(int i = 1; i < notesOfChord.size(); i++) {
                if(notes[prevNote] >= notes[notesOfChord[i]]) {
                    oct++;
                }
                prevNote = notesOfChord[i];

                notesOfChord[i] = notesOfChord[i] + to_string(oct);
            }
        }

        void matchNoteTimeAndDuration(Note n) {
            time = n.time;
            duration = n.duration;
        }
};

//Function Declarations
void initCharts();
vector<string> getNeighborKeys(string);
void fillChart(int, int);
void constructChartOfRegions(string);
void printChartOfRegions();
void inputPitches();

vector<string> getNotesOfScale(string, string);
vector<string> getNotesOfScale();


/**Simple harmonizations*/
void Method0MainTones();
vector<Chord> METHOD0();
vector<Chord> METHOD1();

/**Schoenberg harmonizations*/
vector<Chord> schoenberg(vector<int>, vector<int>, float, int, bool, bool, int, int);
vector<Chord> METHOD2();
vector<Chord> METHOD3();
vector<Chord> METHOD4();

/**Mode Harmonizations*/
vector<Chord> modeHarmonization(string, float, int);
vector<Chord> specialChordsOfMode(string, vector<string>);
vector<Chord> METHOD5();
//vector<Chord> METHOD6();
vector<Chord> METHOD7();
vector<Chord> METHOD8();
vector<Chord> METHOD9();
vector<Chord> METHOD10();


void chordWriter();
void writeToChords();

void writeToMelody();
int pitchToN(float);
string NtoNote(int);
void formatPitches();
int calculateAverageMelodyOctave();
void getNotesFromMelody(string);
void getKeyAndModeFromMelody(string);


//Charts/Maps/Look-up Tables

map<string, int> modes;
map<int, string> BasicChords;

//Every mode has a note that makes it sound the way it does
map<string, pair<int, int> > specialNoteOfMode;

//The reason having a literal chart data structure could be useful
//is traversing the chart and coming back to a certain note
//Also lowercase is minor, uppercase is major
//It will be oriented so that (0, 0) is the upper-left corner
string chartOfRegions[25][25];


//GLOBAL VARIABLES

// Defaults should match melodyJsFile
// TODO: Grep these from melodyJsFile 
float songDuration = 26;
string key = "F";        
string mode = "major";
float bpm = 125; 

//Play back method 0 then method 1 of harmonization
vector<int> harmonizationsToPlay; 
//1 second wait between different harmonies/melodies
const float timeBetweenVersions = 1;
//When the harmony/melody starts playing
const float startTime = 2;

float measureDuration;
//The octave the melody most appears in
int averageMelodyOctave;

int melodyPitchesLength;
vector<float> melodyPitches;

vector<Note> notesOfMelody;
vector<vector<Chord> > harmonizations;
vector<string> SCALE;

void messagesToPrint(int);

//You can't insert elements outside of a function
void initCharts() {
    time_t t = time(0);
    srand(3);
    
    modes["major"] = 1;
    modes["dorian"] = 2;
    modes["phrygian"] = 3;
    modes["lydian"] = 4;
    modes["mixolydian"] = 5;
    modes["minor"] = 6;
    modes["locrian"] = 7;

    BasicChords[1] = "IChord";
    BasicChords[2] = "iiChord";
    BasicChords[3] = "iiiChord";
    BasicChords[4] = "IVChord";
    BasicChords[5] = "VChord";
    BasicChords[6] = "viChord";
    BasicChords[7] = "viiChord";

    //The first element in the pair is the special note
    //The second element is the # of semitones away from the modal tonic it is
    specialNoteOfMode["dorian"] = make_pair(6, 9);
    specialNoteOfMode["phrygian"] = make_pair(2, 1);
    specialNoteOfMode["lydian"] = make_pair(4, 6);
    specialNoteOfMode["mixolydian"] = make_pair(7, 10);
    specialNoteOfMode["locrian"] = make_pair(5, 6);
}

void messagesToPrint(int methodNum) {
    switch(methodNum) {
        case 0:
            cout << endl << "--------Simple Harmonization 1--------" << endl << endl;
            break;
        case 1:
            cout << endl << "--------Simple Harmonization 2--------" << endl << endl;
            break;
        case 2:
            cout << endl << "--------Schoenberg Minimum Method--------" << endl << endl;
            break;
        case 3:
            cout << endl << "--------'Giant Steps' Method--------" << endl << endl;
            break;
        case 4:
            cout << endl << "--------Schoenberg Maximum Method--------" << endl << endl;
            break;
        case 5:
            cout << endl << "--------Random mode harmonization--------" << endl << endl;
            break;      
        case 6:
            cout << endl << "--------Dorian mode harmonization--------" << endl << endl;
            break; 
        case 7:
            cout << endl << "--------Phrygian Dominant mode harmonization--------" << endl << endl;
            break; 
        case 8:
            cout << endl << "--------Lydian mode harmonization--------" << endl << endl;
            break; 
        case 9:
            cout << endl << "--------Mixolydian mode harmonization--------" << endl << endl;
            break; 
        case 10:
            cout << endl << "--------Locrian mode harmonization--------" << endl << endl;
            break;             
    }
    
}


void getMelodyFromMusicxml(string musicxmlFile) {
    string fname = "../inputMusic/" + musicxmlFile;
    // The sed's below remove ending tags,  spaces and empty lines
    string command = "egrep -i 'fifths|beat-unit|divisions|per-minute|rest|pitch|step|alter|octave|duration|<tie ' " + fname +
        "| sed -e 's@</fifths>@@g' "     +
        "| sed -e 's@</per-minute>@@g' " +
        "| sed -e 's@</beat-unit>@@g' "  +
        "| sed -e 's@</divisions>@@g' "  +
        "| sed -e 's@</duration>@@g' "   +
        "| sed -e 's@</step>@@g' "       +
        "| sed -e 's@</alter>@@g' "       +
        "| sed -e 's@</octave>@@g' "     +
        "| sed -e 's@</pitch>@@g' "      +
        "| sed -e 's@ @@g' "             +
        "| sed '/^$/d' > j3";
    int statusEgrep = system(command.c_str()); //cout << "statusEgrep: " << statusEgrep << "\n";

    FILE *stream = freopen("j3", "r", stdin);
    assert (stream != NULL && "Couldn't open j3\n");
    
    string line, state = "needDivisions", pitch = "";
    float divisions = 0.0, perMin = 0.0, durationFactor = 0.0;
    int fifths = -1, restDuration = 0, pitchDuration = 0;
    string out = "const melody = [\n";
    float sT = startTime, lastTime = 0, time = 0;
    //                           -6    -5    -4    -3    -2   -1    0    1    2    3    4    5
    //                         {"Gb", "Db", "Ab", "Eb", "Bb", "F", "C", "G", "D", "A", "E", "B"}
    string fifths2keyLUT[12] = {"F#", "C#", "G#", "D#", "A#", "F", "C", "G", "D", "A", "E", "B"};
    bool incrementOctave = false;

    while (getline(cin, line)) {
        if (state == "needDivisions") {
            //cout << "in needDivisions\n";
            if (line.find("<divisions>", 0) == string::npos) continue;
            else { // found <divisions>
                stringstream ss(line); string token;
                while(getline(ss, token, '>')) {
                    if (token == "<divisions") continue;
                    divisions = atoi(token.c_str()); 
                    //cout << "divisions = " << divisions << endl;
                    state = "needFifths";
                }                
            }
            if (state == "needFifths") continue;
            else assert(0 && "Didn't find fifths value\n");
        }
        if (state == "needFifths") {
            //cout << "in needFifths\n";
            if (line.find("<fifths>", 0) == string::npos) continue;
            else { // found <fifths>
                stringstream ss(line); string token;
                while(getline(ss, token, '>')) {
                    if (token == "<fifths") continue;
                    fifths = atoi(token.c_str()); 
                    //cout << "fifths = " << fifths << endl;
                    assert (-6 <= fifths && fifths <= 5 && "Invalid fifths\n");
                    key = fifths2keyLUT[6 + fifths];
                    state = "needBeatUnit";
                }                
            }
            if (state == "needBeatUnit") continue;
            else assert(0 && "Didn't find fifths value\n");
        }
        if (state == "needBeatUnit") {
            //cout << "in needBeatUnit\n";
            if (line.find("<beat-unit>", 0) == string::npos) continue;
            else { // found <beat-unit>
                stringstream ss(line); string token;
                while(getline(ss, token, '>')) {
                    if (token == "<beat-unit") continue;
                    //cout << "beat-unit = " << token << endl;
                    assert(token == "quarter" && "Beat-units besides quarters are not yet handled\n");
                    state = "needPerMinute";
                }                
            }
            if (state == "needPerMinute") continue;
            else assert(0 && "Didn't find beat-unit value\n");
        }
        if (state == "needPerMinute") {
            //cout << "in needPerMinute\n";
            if (line.find("<per-minute>", 0) == string::npos) continue;
            else { // found <per-minute>
                stringstream ss(line); string token;
                while(getline(ss, token, '>')) {
                    if (token == "<per-minute") continue;
                    perMin = atoi(token.c_str()); 
                    //cout << "perMin = " << perMin << endl;
                    state = "needRestOrPitch";
                    assert(divisions > 0 && perMin > 0 && "Invalid durations or perMin\n");
                    durationFactor = 60.0 / (divisions * perMin); // duration of 1 division in seconds
                    //cout << "durationFactor = " << durationFactor << endl;
                    assert (durationFactor > 0 && "Invalid durationFactor\n");
                    // time will be multiplied by durationFactor before outputing
                    time = startTime / durationFactor; 
                    //cout << "time = " << time << endl;
                    restDuration = 0; pitchDuration = 0; pitch = "";
                }                
            }
            if (state == "needRestOrPitch") continue;
            else assert(0 && "Didn't find per-minute value\n");
        }
        if ((state == "needRestOrPitch")) {
            //cout << "in needRestOrPitch\n";
            if (line.find("<rest/>", 0) != string::npos) {
                state = "needRestDuration";
                continue;
            } 
            if (line.find("<pitch>", 0) != string::npos) {
                state = "needPitchStep";
                continue;
            }
            assert(0 && "Expect only rest or pitch\n");
        } 
        if (state == "needRestDuration") {
            //cout << "in needRestDuration\n";
            if (line.find("<duration>", 0) == string::npos) assert(0 && "Expect only rest-duration tag\n");
            else { // found <duration> after rest
                stringstream ss(line); string token;
                while(getline(ss, token, '>')) {
                    if (token == "<duration") continue;
                    restDuration += atoi(token.c_str()); 
                    //cout << "restDuration = " << restDuration << endl;
                    state = "gotRestDuration";
                }                
            }
            if (state == "gotRestDuration") continue;
            else assert(0 && "Didn't find rest-duration value\n");
        }
        if (state == "gotRestDuration") {
            //cout << "in gotRestDuration\n";
            if ((line.find("<rest/>", 0) == string::npos) && (line.find("<pitch>", 0) == string::npos)) {
                assert(0 && "Expect either rest or pitch tag\n");
            }
            if (line.find("<rest/>", 0) != string::npos) { // found rest tag
                state = "needRestDuration";
                continue;
            }
            // Found pitch tag, or else would have asserted above
            state = "needPitchStep";
            //cout << "Output rest\n";
            time += restDuration;
            restDuration = 0;
            continue;
        }
        if (state == "needPitchStep") {
            //cout << "in needPitchStep\n";
            if (line.find("<step>", 0) == string::npos) assert(0 && "Expect only step tag\n");
            stringstream ss(line); string token;
            while(getline(ss, token, '>')) {
                if (token == "<step") continue;
                pitch = token;
                state = "needPitchAlterOrOctave";
                //cout << "pitch = " << pitch << endl;
            }
            if (state == "needPitchAlterOrOctave") continue;
            else assert(0 && "Didn't find pitch-step value\n");
        }
        if ((state == "needPitchAlterOrOctave")) {
            cout << "in needPitchAlterOrOctave\n";
            if (line.find("<alter>", 0) != string::npos) {
                stringstream ss(line); string token;
                while(getline(ss, token, '>')) {
                    if (token == "<alter") continue;
                    assert ((token == "1" || token == "-1") && "Invalid alter value\n");
                    if (token == "1") { // sharp
                        pitch += "#";
                        if (pitch == "B#") {
                            pitch = "C";
                            incrementOctave = true;
                        } 
                        if (pitch == "E#") pitch = "F";
                    } else { // flat
                        assert (pitch == "B" && "Only Bflat handled\n");
                        pitch = "A#";
                    }
                    cout << "pitch = " << pitch << endl;
                    state = "needPitchOctave";
                }
                if (state == "needPitchOctave") continue;
                else assert(0 && "Didn't find pitch-alter value\n");
            } 
            if (line.find("<octave>", 0) != string::npos) {
                state = "needPitchOctave";
                // No "continue" here because it should flow proceed to "if (state == needPitchOctave)"
                // without reading the next line, since it needs to process the octave value
                // 
            }
            assert(state ==  "needPitchOctave" && "Expect only alter or octave\n");
        } 
        if (state == "needPitchOctave") {
            //cout << "in needPitchOctave\n";
            if (line.find("<octave>", 0) == string::npos) assert(0 && "Expect only octave tag\n");
            stringstream ss(line); string token;
            while(getline(ss, token, '>')) {
                if (token == "<octave") continue;
                if (incrementOctave) {
                    int octave = atoi(token.c_str());
                    octave++;
                    token = to_string(octave);
                    incrementOctave = false;
                }
                pitch += token;
                //cout << "pitch = " << pitch << endl;
                state = "needPitchDuration";
            }
            if (state == "needPitchDuration") continue;
            else assert(0 && "Didn't find pitch-octave value\n");
        }
        if (state == "needPitchDuration") {
            //cout << "in needPitchDuration\n";
            if (line.find("<duration>", 0) == string::npos) assert(0 && "Expect only pitch-duration tag\n");
            else { // found <duration> after pitch
                stringstream ss(line); string token;
                while(getline(ss, token, '>')) {
                    if (token == "<duration") continue;
                    pitchDuration += atoi(token.c_str()); 
                    //cout << "pitchDuration = " << pitchDuration << endl;
                    state = "gotPitchDuration";
                }
            }
            if (state == "gotPitchDuration") continue;
            else assert(0 && "Didn't find pitch-duration value\n");
        }
        if (state == "gotPitchDuration") {
           // cout << "in gotPitchDuration\n";
            if ((line.find("<rest/>", 0) == string::npos) && (line.find("<pitch>", 0) == string::npos) && (line.find("<tie", 0) == string::npos)) {
                assert(0 && "Expect either rest, pitch or tie tags\n");
            }
            if (line.find("<pitch>", 0) != string::npos) {
                state = "needPitchStep";
                //cout << "Output pitch\n";
                out += " {'time': '" + to_string(time * durationFactor);
                out += "',   'note': '" + pitch;
                out += "', 'duration': '" + to_string(pitchDuration * durationFactor) + "'},\n";
                time += pitchDuration;
                pitch = "";
                pitchDuration = 0;
                continue;
            }
            if (line.find("<rest/>", 0) != string::npos) {
                state = "needRestDuration";
                //cout << "Output pitch\n";
                out += " {'time': '" + to_string(time * durationFactor);
                out += "',   'note': '" + pitch;
                out += "', 'duration': '" + to_string(pitchDuration * durationFactor) + "'},\n";
                time += pitchDuration;
                pitch = "";
                pitchDuration = 0;
                continue;
            } 
            if (line.find("<tietype=\"start\"/>", 0) != string::npos) { // sed has removed all spaces 
                state = "inTie";
                continue;
            } 
        }
        if (state == "inTie") {
            //cout << "in inTie state\n";
            if ((line.find("<pitch>", 0) == string::npos) &&
                (line.find("<step>", 0) == string::npos) &&
                (line.find("<alter>", 0) == string::npos) &&
                (line.find("<octave>", 0) == string::npos) &&
                (line.find("<duration>", 0) == string::npos) &&
                (line.find("<tie", 0) == string::npos)) { // <tietype="stop"/>
                assert(0 && "Expect either pitch, step, alter, octave, duration or tie stop\n");
            }
            if (line.find("<duration>", 0) != string::npos) {
                stringstream ss(line); string token;
                while(getline(ss, token, '>')) {
                    if (token == "<duration") continue;
                    pitchDuration += atoi(token.c_str()); 
                    //cout << "pitchDuration = " << pitchDuration << endl;
                }
                continue;
            }
            if (line.find("<tietype=\"stop\"/>", 0) != string::npos) { // sed has removed all spaces 
                state = "needRestOrPitch";
                //cout << "Output pitch\n";
                out += " {'time': '" + to_string(time * durationFactor);
                out += "',   'note': '" + pitch;
                out += "', 'duration': '" + to_string(pitchDuration * durationFactor) + "'},\n";
                time += pitchDuration;
                pitch = "";
                pitchDuration = 0;
                continue;
            } 
            // Ignore pitch, step, alter, octave tags in tie, assuming the pitch doesn't change. So don't check them
        }
    }  // while getline

    out += "];\n"; 
    // out += "const key = 'F';\n";
    // out += "const mode = 'major';\n";
    out += "const key = '"  + key  + "';\n";
    out += "const mode = '" + mode + "';\n";

    ofstream melody("../playNotes/melody.js");
    melody << out; 
    melody.close();
}

void getNotesFromMelody(string melodyJsFname) {
    string command = "grep time ../playNotes/" + melodyJsFname +
        " | awk -F \" \" '{print $2 $4 $6}' | sed -e 's/,/ /g' |  sed -e 's/}/ /g' |  sed -e \"s/'/ /g\" > j2";
    int statusGrepAwkSed = system(command.c_str()); //cout << "statusGrepAwkSed: " << statusGrepAwkSed << "\n";
    float time, duration;
    char note[10];
    FILE *fdIn = fopen("j2", "r");
    while (fscanf(fdIn, "%f %s %f", &time, note, &duration) == 3) {
        //printf("%f %s %f\n", time, note, duration);
        Note val;
        val.time = time;
        val.note = note;
        val.duration = duration;
        notesOfMelody.push_back(val);
    }
    fclose(fdIn);
}

void getKeyAndModeFromMelody(string melodyJsFname) {
    string command = "grep key ../playNotes/"  + melodyJsFname + " | awk -F \" \" '{print $4}' | sed -e \"s/'//g\" | sed -e 's/;//g' > j2";
    int statusGrepAwkSed = system(command.c_str()); //cout << "statusGrepAwkSed: " << statusGrepAwkSed << "\n";
    command        = "grep mode ../playNotes/" + melodyJsFname + " | awk -F \" \" '{print $4}' | sed -e \"s/'//g\" | sed -e 's/;//g' >> j2";
    statusGrepAwkSed     = system(command.c_str()); //cout << "statusGrepAwkSed: " << statusGrepAwkSed << "\n";
    std::ifstream fIn;
    fIn.open("j2");
    if (fIn >> key) cout << "key: " << key << "\n";
    if (fIn >> mode) cout << "mode: " << mode << "\n";
    fIn.close();
}



void inputPitches() {
    freopen("../blobs/pitches.txt", "r", stdin);
    
    //pitches.txt format:
    //durationInSec key majmin bpm arrayLength pitch(1) pitch(2) pitch(3) .... pitch(arrayLength)

    cin >> songDuration;
    cout << "songDuration: " << songDuration << endl;
    cin >> key;

    map<string, string> flatToSharp;
    flatToSharp["Cb"] = "B";
    flatToSharp["Db"] = "C#";
    flatToSharp["Eb"] = "D#";
    flatToSharp["Fb"] = "E";
    flatToSharp["Gb"] = "F#";
    flatToSharp["Ab"] = "G#";
    flatToSharp["Bb"] = "A#";

    if(flatToSharp.count(key) > 0) {
        key = flatToSharp[key];
    }

    cout << "key: " << key << endl;
    cin >> mode;
    cout << "maj/min: " << mode << endl;
    cin >> bpm;
    cout << bpm << endl;
                       
    cin >> melodyPitchesLength;
    assert((melodyPitchesLength > 0) && (melodyPitchesLength < 100000) && "Invalid melodyPitchesLength\n");
    melodyPitches.resize(melodyPitchesLength);

    for(int i = 0; i < melodyPitchesLength; i++) {
        cin >> melodyPitches[i];
    }
}

//Chart of Regions Functions=========================================================================================================

void constructChartOfRegions(string initialKey) {
    //u means unfilled
    for(int i = 0; i < sizeof(chartOfRegions)/sizeof(chartOfRegions[0]); i++) {
        for(int j = 0; j < sizeof(chartOfRegions[0])/sizeof(chartOfRegions[0][0]); j++) {
            chartOfRegions[i][j] = "u";
        }
    }
     
    //initial key will be in the center at (12, 12)
    chartOfRegions[12][12] = initialKey;

    fillChart(12, 12);
}

void fillChart(int x, int y) {

    //If (0, 0) is the upper-left corner, 
    //These directions are down, right, up, left
    int xDif[4] = {0, 1, 0, -1};
    int yDif[4] = {1, 0, -1, 0};

    vector<string> neighborKeys = getNeighborKeys(chartOfRegions[x][y]);

    for(int p = 0; p < 4; p++) {
        int xx = xDif[p] + x;
        int yy = yDif[p] + y;

        if(0 <= xx && xx < 25 && 0 <= yy && yy < 25 && chartOfRegions[xx][yy] == "u") {
            chartOfRegions[xx][yy] = neighborKeys[p];

            fillChart(xx, yy);
        }
    }
}

vector<string> getNeighborKeys(string k) {
    bool major = true;
    //The key should be lowercase if it's minor
    if(islower(k[0])) major = false;

    k[0] = toupper(k[0]);

    vector<string> notesOfScale = (major ? getNotesOfScale(k, "major") : getNotesOfScale(k, "minor"));

    vector<string> res; res.resize(4);

    //Down should be the major 4th
    //Right should be the minor 1st
    //Up should be the major 5th
    //Left should be the minor 6th
    if(major) {
        string maj4 = notesOfScale[3];
        string min1 = notesOfScale[0]; min1[0] = tolower(min1[0]);
        string maj5 = notesOfScale[4];
        string min6 = notesOfScale[5]; min6[0] = tolower(min6[0]);

        res[0] = maj4;
        res[1] = min1;
        res[2] = maj5;
        res[3] = min6;
    }
    //Down should be the minor 4th
    //Right should be the major 3rd
    //Up should be the minor 5th
    //Left should be the major 1st
    else {
        string min4 = notesOfScale[3]; min4[0] = tolower(min4[0]);
        string maj3 = notesOfScale[2]; 
        string min5 = notesOfScale[4]; min5[0] = tolower(min5[0]);
        string maj1 = notesOfScale[0]; 

        res[0] = min4;
        res[1] = maj3;
        res[2] = min5;
        res[3] = maj1;
    }

    return res;
}

void printChartOfRegions() {
    for(int i = 0; i < 25; i++) {
        for(int j = 0; j < 25; j++) {
            if(chartOfRegions[j][i].size() > 1) {
                cout << chartOfRegions[j][i] << " ";
            }
            else {
                cout << chartOfRegions[j][i] << "  ";
            }
        }
        cout << endl;
    }
}

//FUNCTIONS TO WRITE CHORDS=========================================================================================================

vector<string> getNotesOfScale(string k, string m) {
    //phrygian dominant, phrygian except the 3rd is major
    //this mode is easier to harmonize than phrygian
    vector<int> intervals;
    
    if(m == "phrygianD") {
        int ints [] = {0, 1, 4, 5, 7, 8, 10};
        intervals.insert(intervals.begin(), ints, ints + 7);
    }
    else {
        char scalePattern [] = {'W', 'W', 'H', 'W', 'W', 'W', 'H'};

        int index = (int) (modes[m]) - 1;

        intervals.resize(7);
        intervals[0] = 0;

        for(int i = 1; i < 7; i++) {
            int increment = 1;
            increment += (scalePattern[index] == 'W');
            
            intervals[i] = intervals[i-1] + increment;

            index = (index + 1)%7;
        }
    }


    string notes [] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};
    int start = 0;

    for(int i = 0; i < 12; i++) {
        if(k == notes[i]) start = i;
    }

    vector<string> notesOfScale; notesOfScale.resize(7);

    int index;
    for(int i = 0; i < 7; i++) {
        index = intervals[i] + start;

        notesOfScale[i] = notes[index%12];
    }

    return notesOfScale;
}

vector<string> getNotesOfScale() {
    //Use global mode and key
    return getNotesOfScale(key, mode);
}

/**
 * Harmonization Method 0 
 * 
 * We first calculate main tones as such:
 * Assume 1 and 5 are always main tones
 * A note is a main tone if its "emphasis" is greater than its neighbors
 * Emphasis is the ratio of a note's duration to the average note duration
 * 
 * Assign a chord to every main tone such that the 
 * tone is the 3rd of the triad (1,3,5) 
 * 
 * 1 still gets a IChord and 5 still gets a VChord
 * to ensure it doesn't sound minor
 */

void Method0MainTones() {
    //Current method: 
    //Assume 1, 5 is main tone
    
    //tones with emphases greater than its 2 neighbors
    //(tones that are like a local maximum emphasis) are main tones

    //If you play the same note multiple times, only the first note should be a main tone
    
    //Notes that are not in the scale are automatically passing tones
    //(for now we ignore accidentals)

    float totalNoteDuration = 0;

    for(int i = 0; i < notesOfMelody.size(); i++) {
        //getRelativeNote sets mainTone to false if note is not in the scale
        float relativeNote = notesOfMelody[i].getRelativeNote(SCALE);

        if(relativeNote != 1 && relativeNote != 5) {
            notesOfMelody[i].mainTone = false;
        }


        totalNoteDuration += notesOfMelody[i].duration;
    }

    float note1 = notesOfMelody[0].getEmphasis(totalNoteDuration, notesOfMelody.size());
    float note2 = notesOfMelody[1].getEmphasis(totalNoteDuration, notesOfMelody.size());

    float note2ndToLast = notesOfMelody[notesOfMelody.size() - 2].getEmphasis(totalNoteDuration, notesOfMelody.size());
    float noteLast = notesOfMelody[notesOfMelody.size() - 1].getEmphasis(totalNoteDuration, notesOfMelody.size());

    if(note1 > note2 && notesOfMelody[0].getRelativeNote(SCALE) != -1) {
        notesOfMelody[0].mainTone = true;
    }
    else if(noteLast > note2ndToLast && notesOfMelody[notesOfMelody.size() - 1].getRelativeNote(SCALE) != -1) {
        notesOfMelody[notesOfMelody.size() - 1].mainTone = true;
    }

    for(int i = 1; i < notesOfMelody.size() - 1; i++) {
        //getRelativeNote sets mainTone to false if note is not in the scale
        float prevEmphasis = notesOfMelody[i-1].getEmphasis(totalNoteDuration, notesOfMelody.size());
        float curEmphasis = notesOfMelody[i].getEmphasis(totalNoteDuration, notesOfMelody.size());
        float nextEmphasis = notesOfMelody[i+1].getEmphasis(totalNoteDuration, notesOfMelody.size());

        if(curEmphasis > prevEmphasis && curEmphasis > nextEmphasis && notesOfMelody[i].getRelativeNote(SCALE) != -1) {
            notesOfMelody[i].mainTone = true;
        }
    }

    vector<pair<int, int> > consecutiveSameNotes;
    int p1 = -1; int p2 = -1;
    int curNote = -1;

    for(int i = 0; i < notesOfMelody.size(); i++) {
        if(notesOfMelody[i].getRelativeNote(SCALE) != curNote) {
            if(p1 != -1) {
                p2 = i - 1;

                if(p1 != p2) {
                    pair<int, int> pear; pear.first = p1; pear.second = p2;
                    consecutiveSameNotes.push_back(pear);
                }
            }
            
            p1 = i; p2 = -1;
            curNote = notesOfMelody[i].getRelativeNote(SCALE);
        }
    }

    for(int i = 0; i < consecutiveSameNotes.size(); i++) {
        int note = notesOfMelody[consecutiveSameNotes[i].first].getRelativeNote(SCALE);
        if(note == -1) continue;

        notesOfMelody[consecutiveSameNotes[i].first].mainTone = true;        
        
        for(int j = consecutiveSameNotes[i].first + 1; j <= consecutiveSameNotes[i].second; j++) {
            notesOfMelody[j].mainTone = false; 
        }
    }

}


vector<Chord> METHOD0() {
    Method0MainTones();
    vector<Chord> res;

    for(int i = 0; i < notesOfMelody.size(); i++) {
        Note curNote = notesOfMelody[i];

        if(curNote.mainTone) {
            Chord c; c.matchNoteTimeAndDuration(curNote);
            if(curNote.getRelativeNote(SCALE) == 1) {
                c.chord = "IChord";
            }
            else if(curNote.getRelativeNote(SCALE) == 5) {
                c.chord = "VChord";
            }
            else {
                assert((curNote.getRelativeNote(SCALE) != -1) && "error in METHOD0");
                c.chord = BasicChords[((int) curNote.getRelativeNote(SCALE) + 4) % 7 + 1];
            }
            //make chord last until next main tone
            for(int j = i + 1; j < notesOfMelody.size(); j++) {
                if(notesOfMelody[j].mainTone == true) {
                    c.duration = notesOfMelody[j].time - notesOfMelody[i].time;
                    break;
                }
                else if(j == notesOfMelody.size() - 1) {
                    c.duration = notesOfMelody[j].duration + notesOfMelody[j].time - notesOfMelody[i].time;
                }
            }


            res.push_back(c);
        }
    }

    return res;
}

/**
 * Harmonization Method 1 
 * 
 * We assume all non-accidentals are main tones
 * 
 * Assign a chord to every main tone such that the 
 * tone is the 3rd of the triad (1,3,5) 
 */ 

vector<Chord> METHOD1() {
    vector<Chord> res;

    for(int i = 0; i < notesOfMelody.size(); i++) {
        Note curNote = notesOfMelody[i];

        if(curNote.getRelativeNote(SCALE) != -1) {
            Chord c; 
            c.matchNoteTimeAndDuration(curNote);
            c.chord = BasicChords[((int) curNote.getRelativeNote(SCALE) + 4) % 7 + 1];

            res.push_back(c);
        }
    }
    return res;
}

/**
 * Harmonization Method 2
 * 
 *  Jump through the chart of regions randomly
 *  Go to secondary dominant, end on tonic at the end
 *  
 * 
 * For now, we will use the same main tones as method0
 * Later, we will use BPM and song duration to find the measures
 * 
 */ 
vector<Chord> METHOD2() {
    int xd[] = {0, 1, 0, -1};
    int yd[] = {1, 0, -1, 0};

    vector<int> xDif; xDif.insert(xDif.begin(), xd, xd + 4);
    vector<int> yDif; yDif.insert(yDif.begin(), yd, yd + 4);

    return schoenberg(xDif, yDif, 2 * measureDuration, 4, false, true, 100, 25);
}

//giant steps
vector<Chord> METHOD3() {
    int xd[] = {0, 0};
    int yd[] = {4, -4}; 

    vector<int> xDif; xDif.insert(xDif.begin(), xd, xd + 2);
    vector<int> yDif; yDif.insert(yDif.begin(), yd, yd + 2);

    return schoenberg(xDif, yDif, measureDuration, 2, false, true, 100, 25);
}

//crazy town
vector<Chord> METHOD4() {
    int xd[] = {1,-1, 0, 0, 2, -2, 0, 0, 1, 1, -1, -1};
    int yd[] = {0, 0, 1,-1, 0, 0, 2, -2, 1, -1, 1, -1};

    vector<int> xDif; xDif.insert(xDif.begin(), xd, xd + 12);
    vector<int> yDif; yDif.insert(yDif.begin(), yd, yd + 12);

    return schoenberg(xDif, yDif, measureDuration/2.0, 1, false, true, 50, 0);
}


vector<Chord> schoenberg(vector<int> xDif, vector<int> yDif, float durationBeforeChange, int repeatsPerChord, bool maintainMode, bool noRepeats, int chanceOfSecDom, int chanceOfII) {
    vector<Chord> res;

    //(12, 12) is the middle of the chart
    int x = 12; int y = 12;

    string origm = (isupper(chartOfRegions[x][y][0]) ? "major" : "minor");

    //Make first chord the tonic
    vector<string> nc; nc.push_back(SCALE[0]); nc.push_back(SCALE[2]); nc.push_back(SCALE[4]); nc.push_back(SCALE[0]);

    Chord c;

    c.notesOfChord = nc; c.addOctaves(max(2, averageMelodyOctave - 2));
    c.duration = durationBeforeChange/repeatsPerChord;

    for(int j = 0; j < repeatsPerChord; j++) {
        c.time = notesOfMelody[0].time + j * c.duration;

        res.push_back(c);
    }

    int prevX [] = {x, x}; int prevY [] = {y, y};
    for(float t = notesOfMelody[0].time + durationBeforeChange; t < notesOfMelody[notesOfMelody.size() - 1].time; t += durationBeforeChange) {
        bool lastTime = false;
        if(t + durationBeforeChange >= notesOfMelody[notesOfMelody.size() - 1].time) lastTime = true;

        string k, m;

        int xx; int yy;
        bool ext = false;
        while(!ext && !lastTime) {
            int dir = rand() % xDif.size();
            xx = ((x + xDif[dir]) + 24) % 24; 
            yy = ((y + yDif[dir]) + 24) % 24;

            k = chartOfRegions[xx][yy];
            m = (isupper(k[0]) ? "major" : "minor"); 
            k[0] = toupper(k[0]);   

            
            bool modeMaintained = true;
            if(maintainMode) {
                if(m != origm) {
                    modeMaintained = false;
                }
            }

            bool noneRepeated = true;
            if(noRepeats) {
                if((prevX[1] == xx && prevY[1] == yy) || (prevX[0] == xx && prevY[0] == yy)) {
                    noneRepeated = false;
                }
            }
            
            if(modeMaintained && noneRepeated) ext = true;
        }
        if(lastTime) {
            xx = 12; yy = 12;

            k = chartOfRegions[xx][yy];
            m = (isupper(k[0]) ? "major" : "minor"); 
            k[0] = toupper(k[0]);   
        }

        prevX[0] = prevX[1]; prevY[0] = prevY[1];
        prevX[1] = x; prevY[1] = y;
        x = xx; y = yy;


        vector<string> curScale = getNotesOfScale(k, m);
        
        //Play dominant [5, 7, 2, 4] then tonic [5, 1, 3] of this scale
        //Occasionally play second chord [6, 2, 4] before dominant
        Chord c;
        string tonic [3] = {curScale[4], curScale[0], curScale[2]};
        string secDom [4] = {curScale[4], curScale[6], curScale[1], curScale[3]};
        string II [3] = {curScale[5], curScale[1], curScale[3]};

        c.notesOfChord.insert(c.notesOfChord.begin(), tonic, tonic+3);
        if(lastTime) {
            c.notesOfChord.clear();
            c.notesOfChord.push_back(curScale[0]); 
            c.notesOfChord.push_back(curScale[2]); 
            c.notesOfChord.push_back(curScale[4]);
            c.notesOfChord.push_back(curScale[0]); 
        }

        c.addOctaves(max(2, averageMelodyOctave - 2));

        //Chord timing
        c.duration = durationBeforeChange/repeatsPerChord;
        for(int j = 0; j < repeatsPerChord; j++) {
            c.time = t + j * c.duration;

            if(rand() % 100 < chanceOfSecDom && j == 0) {
                res[res.size() - 1].duration /= 2;

                Chord x; 
                x.notesOfChord.insert(x.notesOfChord.begin(), secDom, secDom + 4);
                x.addOctaves(max(2, averageMelodyOctave - 2));
                
                x.time = res[res.size() - 1].time + res[res.size() - 1].duration;
                x.duration = res[res.size() - 1].duration;

                res.push_back(x);

                if(rand() % 100 < chanceOfII && repeatsPerChord > 1 && j == 0) {
                    res[res.size() - 2].notesOfChord.insert(res[res.size() - 2].notesOfChord.begin(), II, II + 4);
                    res[res.size() - 2].notesOfChord.resize(3);
                    res[res.size() - 2].addOctaves(max(2, averageMelodyOctave - 2));
                }
            }

            res.push_back(c);
        }

        if(lastTime && repeatsPerChord > 1) {
            res.pop_back();
            res[res.size() - 1].duration *= 2;
        }
    }
    

    return res;
}

/**
 * Mode Harmonizations
 * 
 * Select a mode that is also major or minor
 * Select the mode scale that has the same tonic as the original
 * 
 * Use that mode's special chord progression
 * 
 */ 


vector<Chord> METHOD5() {
    //randomly select matching mode
    if(mode == "major") {
        if(rand() % 2 == 0) {
            return METHOD8();
        }
        return METHOD9();
    }
    
    int choice = rand() % 3;

    switch(choice) {
        case 0: 
            return METHOD4();
        case 1:
            return METHOD7();
    }

    return METHOD10();
}

vector<Chord> METHOD6() {
    return modeHarmonization("dorian", measureDuration, 1);
}

vector<Chord> METHOD7() {
    return modeHarmonization("phrygianD", 2 * measureDuration, 2);
}

vector<Chord> METHOD8() {
    return modeHarmonization("lydian", 2 * measureDuration, 2);
}

vector<Chord> METHOD9() {
    return modeHarmonization("mixolydian", 2 * measureDuration, 2);
}

vector<Chord> METHOD10() {
    return modeHarmonization("locrian", 2 * measureDuration, 2);
}


vector<Chord> modeHarmonization(string modeName, float durationBeforeChange, int repeatsPerChord) {
    vector<string> MODALSCALE = getNotesOfScale(SCALE[0], modeName);

    vector<Chord> chordPattern = specialChordsOfMode(modeName, MODALSCALE);

    vector<Chord> res;    




    int p = 0;
    for(float t = notesOfMelody[0].time; t < notesOfMelody[notesOfMelody.size() - 1].time; t += durationBeforeChange) {
        if(t + durationBeforeChange >= notesOfMelody[notesOfMelody.size() - 1].time) {
            p = 0;
        }
        
        Chord c; c.duration = durationBeforeChange/repeatsPerChord;

        c.notesOfChord = chordPattern[p % chordPattern.size()].notesOfChord;
        c.addOctaves(max(2, averageMelodyOctave - 2));

        p++;

        for(int j = 0; j < repeatsPerChord; j++) {
            c.time = t + j * durationBeforeChange/repeatsPerChord;
            
            res.push_back(c); 
        }
    }


    return res;
}

vector<Chord> specialChordsOfMode(string modeName, vector<string> modalScale) {
    vector<Chord> chords; chords.resize(7);

    for(int i = 0; i < 7; i++) {
        string NOC[3] = {modalScale[i], modalScale[(i+2)%7], modalScale[(i+4)%7]};
        
        Chord c; c.notesOfChord.insert(c.notesOfChord.begin(), NOC, NOC + 3);
        chords[i] = c;
    }

    vector<Chord> res;

    //dorian mode should emphasize i and IV
    //ex: "great gig in the sky" Pink Floyd
    if(modeName == "dorian") {
        Chord i = chords[0];
        Chord ii = chords[1];
        Chord IV = chords[3];

        //ii.notesOfChord.insert(ii.notesOfChord.begin(), modalScale[0]);
        //ii.notesOfChord.push_back(modalScale[0]); 
        //IV.notesOfChord.insert(IV.notesOfChord.begin(), modalScale[0]);

        res.push_back(i); res.push_back(ii); res.push_back(i); res.push_back(IV); 
    }

    //phrygian dominant mode (flamenco/middle eastern sound)
    //the unique part of phrygian is the II chord
    //we use dominant because it resolves better than just phrygian
    //and it sounds really cool and mysterious
    else if(modeName == "phrygianD") {
        Chord I = chords[0];
        Chord II = chords[1];

        res.push_back(II); res.push_back(I);
    }
    else if(modeName == "lydian") {
        Chord I = chords[0];
        Chord II = chords[1];

        II.notesOfChord.push_back(modalScale[0]);

        res.push_back(I); res.push_back(II);
    }
    else if(modeName == "mixolydian") {
        Chord I = chords[0];
        Chord VII = chords[6];

        I.notesOfChord.push_back(modalScale[6]);

        res.push_back(I); res.push_back(VII);
    }
    else if(modeName == "locrian") {    
        //Chord vii = chords[6];
        Chord II = chords[1];
        Chord i_ = chords[0];

        II.notesOfChord.push_back(modalScale[0]);
        II.notesOfChord.push_back(modalScale[0]);
        II.notesOfChord.insert(II.notesOfChord.begin(), modalScale[0]);
        //vii.notesOfChord.push_back(modalScale[0]);

        res.push_back(II); res.push_back(i_); //res.push_back(vii);
    }
    else {
        assert(false && "buh");
    }

    return res;
}

void chordWriter() {
    SCALE = getNotesOfScale();
    cout << "SCALE: ";
    for(int i = 0; i < 7; i++) {
        cout << SCALE[i] << " ";
    }
    cout << endl;

    assert(notesOfMelody.size() > 0 && "notesOfMelody empty (error in chordWriter)");

    harmonizations.push_back(METHOD0());
    harmonizations.push_back(METHOD1());
    harmonizations.push_back(METHOD2());
    harmonizations.push_back(METHOD3());
    harmonizations.push_back(METHOD4());
    harmonizations.push_back(METHOD5());
    harmonizations.push_back(METHOD6());
    harmonizations.push_back(METHOD7());
    harmonizations.push_back(METHOD8());
    harmonizations.push_back(METHOD9());
    harmonizations.push_back(METHOD10());
}

void writeToChords() {
    string out = "\nconst chords = [\n";

    float sT = startTime;

    for(int methodIndex = 0; methodIndex < harmonizationsToPlay.size(); methodIndex++) {
        float lastTime = 0;
        vector<Chord> chordsOfHarmony = harmonizations[harmonizationsToPlay[methodIndex]];

        for(int i = 0; i < chordsOfHarmony.size(); i++) {
            //ex:   {'time': '1.234', 'note': viChord,  'duration': '0.79123', 'velocity': chordVolume},
            
            //if it uses the basic default chords
            if(!chordsOfHarmony[i].chord.empty()) {
                string chords [7] = {"IChord", "iiChord", "iiiChord", "IVChord", "VChord", "viChord", "viiChord"};

                int chordIndex = -1;
                for(int j = 0; j < 7; j++) {
                    if(chordsOfHarmony[i].chord == chords[j]) {
                        chordIndex = j;
                    }
                }
                assert(chordIndex != -1 && "invalid chord (error in writeToChords) ");

                string NOC[3] = {SCALE[chordIndex], SCALE[(chordIndex + 2)%7], SCALE[(chordIndex + 4)%7]};

                chordsOfHarmony[i].notesOfChord.insert(chordsOfHarmony[i].notesOfChord.begin(), NOC, NOC + 3);
                chordsOfHarmony[i].addOctaves(max(3, averageMelodyOctave - 2));
            }

            string s = "[";

            for(int j = 0; j < chordsOfHarmony[i].notesOfChord.size(); j++) {
                string noteName = chordsOfHarmony[i].notesOfChord[j];
                
                assert(65 <= noteName[0] && noteName[0] <= 71 && " not formatted correctly");
                assert(48 <= noteName[noteName.size() - 1] && noteName[noteName.size() - 1] <= 57 && "not formatted correctly");
                
                s += "'" + chordsOfHarmony[i].notesOfChord[j] + "'";

                if(j != chordsOfHarmony[i].notesOfChord.size() - 1) s += ", ";
            }

            s += "]";

            out += " {'time': '" + to_string(chordsOfHarmony[i].time + sT);
            out += "', 'note': " + s;
            out += ", 'duration': '" + to_string(chordsOfHarmony[i].duration);
            out += "', 'velocity': chordVolume},\n";

            lastTime = max(notesOfMelody[notesOfMelody.size() - 1].time + sT + notesOfMelody[notesOfMelody.size() - 1].duration, lastTime);
        }

        sT = lastTime + timeBetweenVersions;
        out += "\n";
    }    

    out += "];\n";

    ofstream chordStream("../playNotes/chords.js");
    chordStream << out; 
    chordStream.close();
}

//FUNCTIONS TO WRITE MELODY=========================================================================================================

void writeToMelody() {
    string out = "const melody = [";
    out += "\n";

    float sT = startTime;

    for(int j = 0; j < harmonizationsToPlay.size(); j++) {
        float lastTime = 0;
        
        for(int i = 0; i < notesOfMelody.size(); i++) {
            //ex: {'time': '1.12353',   'note': 'G4', 'duration': '0.533'},

            out += " {'time': '" + to_string(notesOfMelody[i].time + sT);
            out += "',   'note': '" + notesOfMelody[i].note;
            out += "', 'duration': '" + to_string(notesOfMelody[i].duration);
            out += "', 'velocity': chordVolume * 3},\n";

            lastTime = max(notesOfMelody[i].time + sT + notesOfMelody[i].duration, lastTime);
        }
        sT = lastTime + timeBetweenVersions;
        out += "\n";
    }


    out += "];"; out += "\n";

    out += "const key = '" + key + "';"; out += "\n";
    out += "const mode = '" + mode + "';"; out += "\n";

    ofstream melody("../playNotes/melody.js");
    melody << out; 
    melody.close();
}


int pitchToN(float pitch) {
    float unrounded = 12 * log2 (pitch/440);

    //round it to nearest integer
    int lb = (int) unrounded;
    int ub = lb + 1;

    if( ((float) ub) - unrounded < unrounded - ((float) lb)) {
        return ub;
    }
    return lb;
}

string NtoNote(int n) {
    string notes [] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};

    //We use (b + (a%b)) % b to not get negative numbers
    // -- a%b returns numbers in the range [-(b-1) ,  b-1] due to the way mod is defined in c++
    // -- b + (a%b) makes it in the range [1 , 2b-1]  
    //          We essentially shifted the remained into a guaranteed positive range
    // -- (b + (a%b)) % b makes it in the range [0, b-1] 
    //          By taking the modulus (remainder of b) again, we confine the range back to between 0 and b-1
    //          since a remainder can't be more than b-1

    int index = (12 + (n % 12)) % 12;
    
    //printf ("n = %d, index = %d\n", n, index);
    assert((index >= -11) && (index <= 11) && "Invalid index\n");
    //index = (index < 0) ? (index + 12) : 0;
    string note = notes[index];
    assert((n >= -45) && (n <= 39) && "Invalid n\n");
    string octave = to_string((n + 69)/12 - 1);

    return note + octave;
}



void formatPitches() {
    vector<string> notes;
    //assert((melodyPitchesLength > 0) && (melodyPitchesLength < 10000) && "Invalid melodyPitchesLength\n");
    notes.resize(melodyPitchesLength);

    for(int i = 0; i < melodyPitchesLength; i++) {
        if(melodyPitches[i] == 0) {
            notes[i] = "NOTHING";
            continue;
        }
        
        notes[i] = NtoNote(pitchToN(melodyPitches[i]));
    }

    vector<pair<string, float> > noteLength;

    string prev = notes[0];
    string cur;
    int count = 0;

    for(int i = 0; i < melodyPitchesLength;i++) {
        cur = notes[i];

        if(i == melodyPitchesLength - 1) {
            pair<string, float> p;
            p.first = prev;
            p.second = (float) count;

            noteLength.push_back(p);
        }
        else if(cur == prev) {
            count++;
        }
        else {
            pair<string, float> p;
            p.first = prev;
            p.second = (float) count;

            noteLength.push_back(p);

            count = 0;
        }

        prev = cur;
    }

    for(int i = 0; i < noteLength.size(); i++) {
        noteLength[i].second = songDuration * noteLength[i].second/ (float) melodyPitchesLength;
    }

    //Time and duration can both either be in seconds or as a tempo-relative value
    //Below implementation utilizes seconds to keep things simple
    //We can always just assume when the measure or beat is in the future
    //Potentially use another Essentia algorithm to find BPM

    float t = 0;

    for(int i = 0; i < noteLength.size(); i++) {
        string curNote = noteLength[i].first;
        float curTime = noteLength[i].second;


        if(curNote != "NOTHING") {
            Note val;
            val.time = t;
            val.note = curNote;
            val.duration = curTime;

            notesOfMelody.push_back(val);
        }

        t += curTime;
    }
}

int calculateAverageMelodyOctave() {
    float total = 0.0;
    for(int i = 0; i < notesOfMelody.size(); i++) {
        total += notesOfMelody[i].getOctave();
    }

    float average = total/ notesOfMelody.size();

    //effectively rounds average
    return (int) average + 0.5;
}

//MAIN==============================================================================================================================

int main(int argc, char** argv) {
    //cout << "You have entered " << argc << " arguments:" << "\n";

    if ((argc != 2) && (argc != 3) && (argc != 4)) {
        cout << "Usage: ./harmonizer mode filename major/minor\n";
        cout << "mode = 0 => use microphone input\n";
        cout << "       1 => use inputMusic/filename as .wav input\n";
        cout << "       2 => use inputMusic/input.wav as input\n";
        cout << "       3 => use playNotes/filename as .js input\n";
        cout << "       4 => use playNotes/filename as .js input. Generate files but skip audio output \n";
        cout << "       5 => use inputMusic/filename as .musicxml input \n";
        cout << "Examples:\n";
        cout << "./harmonizer 0\n";
        cout << "./harmonizer 1 fMajScale.wav\n";
        cout << "./harmonizer 2\n";
        cout << "./harmonizer 3 melodyHBf.js\n";
        cout << "./harmonizer 4 melodyHBf.js\n";
        cout << "./harmonizer 5 melodyHBf.musicxml major\n";
        cout << "./harmonizer 5 melodyHBf.musicxml minor\n";
        exit(0);
    }
    //for (int i = 0; i < argc; ++i) cout << argv[i] << "\n";
    bool useMicInput = false;
    bool useMelodyInput = false;
    bool playAudio = true;
    bool useMusicxml = false;
    string inputWavFile = "input.wav";
    string melodyJsFile = "melody.js";
    string musicxmlFile = "hb.musicxml";
    const int argv1 = stoi(argv[1]);
    switch (argc) {
    case 2: {
        assert((argv1 == 0) || (argv1 == 2));
        if (argv1 == 0) {
            useMicInput = true;
        } 
        break;
    }
    case 3: {
        switch (argv1) {
        case 1: {
            inputWavFile = argv[2];            
            break;
        }
        case 3: {
            useMelodyInput = true;
            melodyJsFile = argv[2];
            break;
        }
        case 4: {
            useMelodyInput = true;
            melodyJsFile = argv[2];
            playAudio = false;
            break;
        }
        default:
            assert(0 && "Invalid argv[1]\n");
            break;
        }
        break;
    }
    case 4: {
        assert(argv1 == 5);
        useMusicxml = true;
        musicxmlFile = argv[2];
        mode = argv[3];
        break;
    }
    default:
        assert(0 && "Invalid argc\n");
        break;
    }
    // cout << "useMicInput: " << useMicInput << "\n";
    // cout << "inputWavFile: " << inputWavFile << "\n";
    // cout << "melodyJsFile: " << melodyJsFile << "\n";
    // cout << "playAudio: " << playAudio << "\n";
    // cout << "mode: " << mode << "\n";

    initCharts();

    if (useMelodyInput == false) {
        if (useMusicxml == false) {
        int statusGetAudio = 0;
        if (useMicInput) {
            printf ("RECORDING 5 seconds of audio from the microphone (device 0) !!!!\n");
            statusGetAudio = system("cd ../inputMusic/; rm -f input.wav; ./ffmpeg -f avfoundation -i \":0\" -t 5 input.wav");
        } else {
            if (inputWavFile != "input.wav") {
                string command = "cd ../inputMusic/; cp " + inputWavFile + " input.wav";
                statusGetAudio = system(command.c_str());
            }
        }
        //printf ("statusGetAudio: %d\n", statusGetAudio);    

        //system("open ../inputMusic/input.wav");

        //Use Essentia pitch detection to detect pitches
        int statusGetPitches = system("node ../getNotes/getPitches.js");
        //printf ("statusGetPitches: %d\n", statusGetPitches);    

        inputPitches(); 
        formatPitches();
        //TODO: Melody Smoothener: Something that detects when a note is ambiguous and estimates it to one note
        //instead of allowing it to rapidly fluctuate between 2 adjacent notes, creating an uncomfortable mess of a melody
        
        //melodySmoothener();
        } else { // useMusicxml == true
            getMelodyFromMusicxml(musicxmlFile);
            getNotesFromMelody(melodyJsFile);
            getKeyAndModeFromMelody(melodyJsFile);
        }
    } else { // useMelodyInput == true
        getNotesFromMelody(melodyJsFile);
        getKeyAndModeFromMelody(melodyJsFile);



        // assume bpm for happy birthday
        bpm = 120;
        songDuration = 26.0;
    }

    if(mode == "minor") {
        int HTP[] = {2, 7, 3, 0, 6, 4, 10, 1};
        harmonizationsToPlay.insert(harmonizationsToPlay.begin(), HTP, HTP + 8);
    }
    else {
        int HTP[] = {2, 8, 3, 0, 9, 4, 1};
        harmonizationsToPlay.insert(harmonizationsToPlay.begin(), HTP, HTP + 7);
    }

    writeToMelody(); 




    string k = key;
    if(mode == "minor") k[0] = tolower(k[0]);
    measureDuration = 4 / (bpm / 60.0);
    averageMelodyOctave = calculateAverageMelodyOctave();

    constructChartOfRegions(k);
    printChartOfRegions();
    exit(0);  
    chordWriter();
    for(int i = 0; i < harmonizationsToPlay.size(); i++) {
        cout << endl << "HARMONIZATION " << to_string(i + 1);
        messagesToPrint(harmonizationsToPlay[i]);
    }

    writeToChords();
    
    //int statusCreateMusic = system("cd ../playNotes/; rm -f music.js; cat musicFirst.js melody.js musicLast.js > music.js");
    int statusCreateMusic = system("cd ../playNotes/; rm -f music.js; cat musicFirst.js melody.js chords.js musicLast.js > music.js");
    //printf ("statusCreateMusic: %d\n", statusCreateMusic);    

    if (playAudio) {
        //Run server, if not already running
        int serverAlreadyRunning = system("ps > jj; grep -v \"grep\" jj > j3 | grep \"SimpleHTTPServer\" j3"); system("rm -f jj j3");
        //printf ("serverAlreadyRunning: %d\n", serverAlreadyRunning); 
        if (serverAlreadyRunning != 0) {
            int statusRunServer = system("cd ../; python -m SimpleHTTPServer 4000 2> logServer.txt &");
            //printf ("statusRunServer: %d\n", statusRunServer);
        }
        int statusPlayMusic = system("open -a firefox http://localhost:4000/playNotes/test.html");
        //printf ("statusPlayMusic: %d\n", statusPlayMusic);    
    }
    return 0;
}
