# Simon Game CAB202 2023

## Gameplay
- Upon system reset or starting a new game, the sequence length is set to one.
- "Simon" will play a sequence of tones using the buzzer and light up corresponding segments of the 7-segment display.
- Each tone and corresponding segment will be lit for 50% of the duration of the playback delay.
- The user must press the pushbuttons corresponding to the sequence.
- If the user's sequence matches Simon's, the SUCCESS pattern is displayed, followed by the user's score, and the sequence length increases by one.
- The next sequence will replicate the notes from the previous sequence and add an additional note at the end.
- The game continues until the user fails to match Simon's sequence or the maximum sequence length is reached.
- If the user fails, the FAIL pattern is displayed, the score is shown, and a new sequence starts with a sequence length of one.
- The user's score is displayed as a decimal number between 0 and 99.

## The Sequence
- The sequence is generated from a pseudo-random sequence seeded by the digits of the student number.
- This sequence will be generated with the following algorithm
<pre><code>BIT ← lsbit(STATE_LFSR)
STATE_LSFR ← STATE_LFSR >> 1
if (BIT = 1)
    STATE_LFSR ← STATE_LFSR xor MASK
STEP ← STATE_LFSR and 0b11
</code></pre>
<b>Example:</b>
Student number: n12345678 &rarr; STATE_LFSR = 0x12345678
- The pseudo-random sequence seed is reset to its initial value upon system reset.
- If the user succeeds, the next sequence will have the same steps as the previous sequence plus an additional step at the end.
- If the user fails, the game restarts, and a new sequence continues from where the previous sequence ended.
- The pseudo-random number generator is based on a linear-feedback shift register (LFSR) with a specific mask (0xE2023CAB) and initialization based on the student number.

## Playback Delay
- The playback delay is read from the potentiometer and ranges from 0.25 to 2.0 seconds.
- The playback delay controls various aspects of the game.
- The potentiometer can increase or decrease the playback speed.
- The playback speed is interpolated linearly between 0.25 and 2.0 seconds based on the potentiometer position.

## Playback Frequency
- The playback frequencies of the four tones are derived from the student number.
- The octave of the playback frequencies can be increased or decreased through UART commands.
- The frequencies should remain within the range of human hearing (20Hz to 20kHz).

## Gameplay through UART
- Instead of using the pushbuttons, the user can input certain keys in the serial monitor to perform actions.
- The UART interface is configured to 9600—8-N-1.
- The keys can be used to input the pushbutton actions, increment or decrement the frequency, reset the game, or load a new seed for the sequence.

## High Scores
- The game retains a high score table of the top 5 highest scores.
- At the end of a game, if the user's score is within the top 5, they can enter their name.
- The high score table is displayed in descending order, showing the names and scores.
- If the user's score is not within the top 5, the current high score table is displayed.
