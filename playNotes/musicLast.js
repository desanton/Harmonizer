
Tone.loaded().then(() => {
    const part = new Tone.Part(function(time, value){
        sampler.triggerAttackRelease(value.note, value.duration, time, value.velocity);
    }, chords).start(0);
})

const part = new Tone.Part(function(time, note){
    sampler.triggerAttackRelease(note.note, note.duration, time, note.velocity);
}, melody).start(0);

part.humanize = true;
Tone.Transport.start();


throw new Error("Stopping here.");

