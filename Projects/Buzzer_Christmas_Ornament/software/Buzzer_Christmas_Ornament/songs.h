// include the pitches file if it isn't included already
#ifndef PITCHES_h
#include pitches.h
#endif

// Jingle Bells (104 notes)
int jingle_melody[] = {
  // INTRO
  NOTE_D4, NOTE_B4, NOTE_A4, NOTE_G4,
  NOTE_D4, NOTE_D4, NOTE_D4,
  NOTE_D4, NOTE_B4, NOTE_A4, NOTE_G4,
  NOTE_E4, 1,
  NOTE_E4, NOTE_C5, NOTE_B4, NOTE_A4, // o'er the fields we go
  NOTE_FS4, 1,
  NOTE_D5, NOTE_D5, NOTE_C5, NOTE_A4, // laughing all the way
  NOTE_B4,
  NOTE_D4, NOTE_B4, NOTE_A4, NOTE_G4, // bells on bobtail ring
  NOTE_D4, 1,
  NOTE_D4, NOTE_B4, NOTE_A4, NOTE_G4, // making spirits bright
  NOTE_E4, NOTE_E4,
  NOTE_E4, NOTE_C5, NOTE_B4, NOTE_A4, // fun it is to ride and...
  NOTE_D5, NOTE_D5, NOTE_D5, NOTE_D5,
  NOTE_E5, NOTE_D5, NOTE_C5, NOTE_A4,
  NOTE_G4, 1,
  // MELODY
  NOTE_B4, NOTE_B4, NOTE_B4,          // jingle bells...
  NOTE_B4, NOTE_B4, NOTE_B4,
  NOTE_B4, NOTE_D5, NOTE_G4, NOTE_A4, // jingle all the way
  NOTE_B4, 1,
  NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, // oh what fun...
  NOTE_C5, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_B4,
  NOTE_B4, NOTE_A4, NOTE_A4, NOTE_B4,
  NOTE_A4, NOTE_D5, 1,
  NOTE_B4, NOTE_B4, NOTE_B4,          // jingle bells...
  NOTE_B4, NOTE_B4, NOTE_B4,
  NOTE_B4, NOTE_D5, NOTE_G4, NOTE_A4,
  NOTE_B4, 1,
  NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, // oh what fun...
  NOTE_C5, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_B4,
  NOTE_D5, NOTE_D5, NOTE_C5, NOTE_A4,
  NOTE_G4
};

int jingle_tempo[] = {
  // INTRO
  4, 4, 4, 4,
  2, 8, 8,
  4, 4, 4, 4,
  2, 4,
  4, 4, 4, 4,
  2, 4,
  4, 4, 4, 4,
  1,
  4, 4, 4, 4,
  2, 4,
  4, 4, 4, 4,
  2, 4,
  4, 4, 4, 4,
  4, 4, 4, 4,
  4, 4, 4, 4,
  2, 4,
  // MELODY
  4, 4, 2,
  4, 4, 2,
  4, 4, 3, 8,
  2, 1,
  4, 4, 3, 8,
  4, 4, 4, 8, 8,
  4, 4, 4, 4,
  2, 4, 4,
  4, 4, 2,
  4, 4, 2,
  4, 4, 3, 8,
  2, 1,
  4, 4, 3, 8,
  4, 4, 4, 8, 8,
  4, 4, 4, 4,
  2
};

// We Wish You a Merry Christmas (52 notes)
int wish_melody[] = {
  // INTRO
  NOTE_G4,
  NOTE_C5, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4,
  NOTE_A4, NOTE_A4, NOTE_A4,
  NOTE_D5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_C5,
  NOTE_B4, NOTE_G4, NOTE_G4,
  NOTE_E5, NOTE_E5, NOTE_F5, NOTE_E5, NOTE_D5,
  NOTE_C5, NOTE_A4, NOTE_G4, NOTE_G4,
  NOTE_A4, NOTE_D5, NOTE_B4,
  NOTE_C5,
  // Good tidings...
  NOTE_G4,
  NOTE_C5, NOTE_C5, NOTE_C5,
  NOTE_B4, NOTE_B4,
  NOTE_C5, NOTE_B4, NOTE_A4,
  NOTE_G4, NOTE_D5,
  NOTE_E5, NOTE_D5, NOTE_C5,
  NOTE_G5, NOTE_G4, NOTE_G4, NOTE_G4,
  NOTE_A4, NOTE_D5, NOTE_B4,
  NOTE_C5
};

int wish_tempo[] = {
  // INTRO
  4,
  4, 8, 8, 8, 8,
  4, 4, 4,
  4, 8, 8, 8, 8,
  4, 4, 4,
  4, 8, 8, 8, 8,
  4, 4, 8, 8,
  4, 4, 4,
  2,
  // Good tidings...
  4,
  4, 4, 4,
  2, 4,
  4, 4, 4,
  2, 4,
  4, 4, 4,
  4, 4, 8, 8,
  4, 4, 4,
  2
};

// Santa Claus is Coming to Town (112 notes)
int santa_melody[] = {
  NOTE_G4,
  NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_G4,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5,
  NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_G4,
  NOTE_A4, NOTE_G4, NOTE_F4, NOTE_F4,
  NOTE_E4, NOTE_G4, NOTE_C4, NOTE_E4,
  NOTE_D4, NOTE_F4, NOTE_B3,
  NOTE_C4,
  
  NOTE_G4,
  NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_G4,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5,
  NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_G4,
  NOTE_A4, NOTE_G4, NOTE_F4, NOTE_F4,
  NOTE_E4, NOTE_G4, NOTE_C4, NOTE_E4,
  NOTE_D4, NOTE_F4, NOTE_B3,
  NOTE_C4,

  1,

  NOTE_C5,
  NOTE_D5, NOTE_C5, NOTE_B4, NOTE_C5,
  NOTE_A4, NOTE_A4, NOTE_C5,
  NOTE_D5, NOTE_C5, NOTE_B4, NOTE_C5,
  NOTE_A4, NOTE_D5,
  NOTE_E5, NOTE_D5, NOTE_CS5, NOTE_D5,
  NOTE_B4, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_C5,
  NOTE_D5, NOTE_C5, NOTE_B4, NOTE_A4,
  NOTE_G4, 1, NOTE_G4, NOTE_F4,

  NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_G4,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5,
  NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_G4,
  NOTE_A4, NOTE_G4, NOTE_F4, NOTE_F4,
  NOTE_E4, NOTE_G4, NOTE_C4, NOTE_E4,
  NOTE_D4, NOTE_F4, NOTE_D5,
  NOTE_C5
};

int santa_tempo[] = {
  4,
  8, 8, 4, 3, 8,
  8, 8, 4, 2,
  8, 8, 4, 4, 4,
  8, 8, 4, 2,
  4, 4, 4, 4,
  4, 2, 4,
  1,
  
  4,
  8, 8, 4, 3, 8,
  8, 8, 4, 2,
  8, 8, 4, 4, 4,
  8, 8, 4, 2,
  4, 4, 4, 4,
  4, 2, 4,
  1,

  4,

  4,
  4, 4, 4, 4,
  4, 2, 4,
  4, 4, 4, 4,
  2, 4,
  4, 4, 4, 4,
  4, 4, 4, 8, 8,
  4, 4, 4, 4,
  4, 8, 3, 8,

  8, 8, 4, 3, 8,
  8, 8, 4, 2,
  8, 8, 4, 4, 4,
  8, 8, 4, 2,
  4, 4, 4, 4,
  4, 2, 4,
  1
};

// Rudolph the Red-Nosed Reindeer (147 notes)
int rudolph_melody[] = {
  // INTRO
  NOTE_A4, NOTE_B4,
  NOTE_C5, NOTE_A4, NOTE_F4, NOTE_B4, NOTE_G4, NOTE_E4,
  NOTE_A4, NOTE_F4, NOTE_D4, NOTE_A4, NOTE_G4,
  NOTE_C5, NOTE_A4, NOTE_F4, NOTE_B4, NOTE_G4, NOTE_E4,
  NOTE_A4, NOTE_F4, NOTE_D4, NOTE_A4, NOTE_G4,
  NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4,
  NOTE_A4, NOTE_A4, NOTE_B4,
  NOTE_C5, NOTE_C5, NOTE_C5, NOTE_B4, NOTE_A4,
  NOTE_G4,
  // MELODY
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C5, // Rudolph the red-nosed...
  NOTE_A4, NOTE_G4,
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_C5,
  NOTE_B4,
  NOTE_F4, NOTE_G4, NOTE_F4, NOTE_D4, NOTE_B4, // and if you ever saw it...
  NOTE_A4, NOTE_G4,
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_A4,
  NOTE_E4,
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C5, // all of the other reindeer...
  NOTE_A4, NOTE_G4,
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_C5,
  NOTE_B4,
  NOTE_F4, NOTE_G4, NOTE_F4, NOTE_D4, NOTE_B4, // they never let poor Rudolph...
  NOTE_A4, NOTE_G4,
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_D5,
  NOTE_C5,
  NOTE_A4, NOTE_A4, NOTE_C5, NOTE_A4, // then one foggy...
  NOTE_G4, NOTE_E4, NOTE_G4,
  NOTE_F4, NOTE_A4, NOTE_G4, NOTE_F4,
  NOTE_E4,
  NOTE_D4, NOTE_E4, NOTE_G4, NOTE_A4, // "Rudolph with your nose so bright...
  NOTE_B4, NOTE_B4, NOTE_B4,
  NOTE_C5, NOTE_C5, NOTE_B4, NOTE_A4,
  NOTE_G4, NOTE_F4, NOTE_D4,
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C5, // then how the reindeer loved him...
  NOTE_A4, NOTE_G4,
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_C5,
  NOTE_B4,
  NOTE_F4, NOTE_G4, NOTE_F4, NOTE_D4, NOTE_B4, // "Rudolph the red-nosed reindeer...
  NOTE_A4, NOTE_G4,
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_D5,
  NOTE_C5
};

int rudolph_tempo[] = {
  // INTRO
  4, 4,
  4, 2, 4, 4, 2, 4,
  4, 2, 4, 4, 3,
  4, 2, 4, 4, 2, 4,
  4, 2, 4, 4, 3,
  2, 2, 2, 2,
  1, 4, 4,
  2, 2, 4, 2, 4,
  1,
  // MELODY
  8, 4, 8, 4, 4,
  4, 2,
  8, 8, 8, 8, 4, 4,
  1,
  8, 4, 8, 4, 4, // and if you ever saw it...
  4, 2,
  8, 8, 8, 8, 4, 4,
  1,
  8, 4, 8, 4, 4, // all of the other reindeer...
  4, 2,
  8, 8, 8, 8, 4, 4,
  1,
  8, 4, 8, 4, 4, // they never let poor Rudolph...
  4, 2,
  8, 8, 8, 8, 4, 4,
  1,
  4, 4, 4, 4, // then one foggy...
  4, 4, 2,
  4, 4, 4, 4,
  1,
  4, 4, 4, 4, // "Rudolph with your nose so bright...
  4, 4, 2,
  4, 4, 4, 4,
  4, 8, 2,
  8, 4, 8, 4, 4, // then how the reindeer loved him...
  4, 2,
  8, 8, 8, 8, 4, 4,
  1,
  8, 4, 8, 4, 4, // "Rudolph the red-nosed reindeer...
  4, 2,
  8, 8, 8, 8, 4, 4,
  1
};

// Carol Of The Bells (87 notes)
int cotb_melody[] = {
  // INTRO
  NOTE_C5, NOTE_B4, NOTE_C5, NOTE_A4,
  NOTE_C5, NOTE_B4, NOTE_C5, NOTE_A4,
  NOTE_C5, NOTE_B4, NOTE_C5, NOTE_A4,
  NOTE_C5, NOTE_B4, NOTE_C5, NOTE_A4,
  NOTE_E5, NOTE_D5, NOTE_E5, NOTE_C5,
  NOTE_E5, NOTE_D5, NOTE_E5, NOTE_C5,
  NOTE_E5, NOTE_D5, NOTE_E5, NOTE_C5,
  NOTE_E5, NOTE_D5, NOTE_E5, NOTE_C5,
  // MELODY
  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_G5, NOTE_F5,
  NOTE_E5, NOTE_E5, NOTE_E5, NOTE_D5, NOTE_C5,
  NOTE_D5, NOTE_D5, NOTE_D5, NOTE_F5, NOTE_E5,
  NOTE_C5, NOTE_B4, NOTE_C5, NOTE_A4,
  // Merry, Merry, Merry, Merry...
  NOTE_E4, NOTE_FS4, NOTE_GS4, NOTE_A4, NOTE_B4, NOTE_C5,
  NOTE_D5, NOTE_E5, NOTE_D5, NOTE_C5,
  NOTE_E5, NOTE_FS5, NOTE_GS5, NOTE_A5, NOTE_B5, NOTE_C6,
  NOTE_D6, NOTE_E6, NOTE_D6, NOTE_C6,
  // OUTRO
  NOTE_C6, NOTE_B5, NOTE_C6, NOTE_A5,
  NOTE_C6, NOTE_B5, NOTE_C6, NOTE_A5,
  NOTE_C6, NOTE_B5, NOTE_C6, NOTE_A5,
  NOTE_C6, NOTE_B5, NOTE_C6, NOTE_A5
};

int cotb_tempo[] = {
  4, 8, 8, 4,
  4, 8, 8, 4,
  4, 8, 8, 4,
  4, 8, 8, 4,
  4, 8, 8, 4,
  4, 8, 8, 4,
  4, 8, 8, 4,
  4, 8, 8, 4,
  
  4, 8, 8, 8, 8,
  4, 8, 8, 8, 8,
  4, 8, 8, 8, 8,
  4, 8, 8, 4,
  
  8, 8, 8, 8, 8, 8,
  8, 8, 4, 4,
  8, 8, 8, 8, 8, 8,
  8, 8, 4, 4,

  4, 8, 8, 4,
  4, 8, 8, 4,
  4, 8, 8, 4,
  4, 8, 8, 2
};

// Deck the Halls (68 notes)
int deck_melody[] = {
  NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_C4,
  NOTE_D4, NOTE_E4, NOTE_F4, NOTE_D4, NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_B3, NOTE_C4,
  NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_C4,
  NOTE_D4, NOTE_E4, NOTE_F4, NOTE_D4, NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_B3, NOTE_C4,
  NOTE_D4, NOTE_E4, NOTE_F4, NOTE_D4,
  NOTE_E4, NOTE_F4, NOTE_G4, NOTE_D4,
  NOTE_E4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5,
  NOTE_B4, NOTE_A4, NOTE_G4,
  NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_C4,
  NOTE_A4, NOTE_A4, NOTE_A4, NOTE_A4, NOTE_G4, NOTE_F4,
  NOTE_E4, NOTE_D4, NOTE_C4
};

int deck_tempo[] = {
  3, 8, 4, 4,
  4, 4, 4, 4,
  8, 8, 8, 8, 3, 8,
  4, 4, 2,
  3, 8, 4, 4,
  4, 4, 4, 4,
  8, 8, 8, 8, 3, 8,
  4, 4, 2,
  3, 8, 4, 4,
  3, 8, 4, 4,
  8, 8, 4, 8, 8, 4,
  4, 4, 2,
  3, 8, 4, 4,
  4, 4, 4, 4,
  8, 8, 8, 8, 3, 8,
  4, 4, 2
};

// God Rest Ye Merry Gentlemen (68 notes)
int rest_melody[] = {
  NOTE_D4,
  NOTE_D4, NOTE_A4, NOTE_A4, NOTE_G4,
  NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4,
  NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4,
  NOTE_A4, NOTE_D4,
  NOTE_D4, NOTE_A4, NOTE_A4, NOTE_G4,
  NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4,
  NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4,
  NOTE_A4, NOTE_A4,
  NOTE_AS4, NOTE_G4, NOTE_A4, NOTE_AS4,
  NOTE_C5, NOTE_D5, NOTE_A4, NOTE_G4,
  NOTE_F4, NOTE_D4, NOTE_E4, NOTE_F4,
  NOTE_G4,
  1, // I hope this is a rest
  NOTE_F4, NOTE_G4,
  NOTE_A4, NOTE_AS4, NOTE_A4,
  NOTE_A4, NOTE_G4, NOTE_F4, NOTE_E4,
  NOTE_D4, NOTE_F4, NOTE_E4, NOTE_D4,
  NOTE_G4, NOTE_F4, NOTE_G4,
  NOTE_A4, NOTE_AS4, NOTE_C5, NOTE_D5,
  NOTE_A4, NOTE_G4, NOTE_F4, NOTE_E4,
  NOTE_D4
};

int rest_tempo[] = {
  4,
  4, 4, 4, 4,
  4, 4, 4, 4,
  4, 4, 4, 4,
  2, 4,
  4, 4, 4, 4,
  4, 4, 4, 4,
  4, 4, 4, 4,
  2, 4,
  4, 4, 4, 4,
  4, 4, 4, 4,
  4, 4, 4, 4,
  1,
  8, // I hope this is a rest
  4, 4,
  2, 4, 4,
  4, 4, 4, 4,
  2, 8, 8, 4,
  2, 4, 4,
  4, 4, 4, 4,
  4, 4, 4, 4,
  1
};

// O Christmas Tree (62 notes)
int tree_melody[] = {
  NOTE_C4,
  NOTE_F4, NOTE_F4, NOTE_F4, NOTE_G4,
  NOTE_A4, NOTE_A4, NOTE_A4, NOTE_A4,
  NOTE_G4, NOTE_A4, NOTE_AS4, NOTE_E4,
  NOTE_G4, NOTE_F4, NOTE_C4,
  NOTE_F4, NOTE_F4, NOTE_F4, NOTE_G4,
  NOTE_A4, NOTE_A4, NOTE_A4, NOTE_A4,
  NOTE_G4, NOTE_A4, NOTE_AS4, NOTE_E4,
  NOTE_G4, NOTE_F4, 1, NOTE_C5,
  NOTE_C5, NOTE_A4, NOTE_D5, NOTE_C5,
  NOTE_C5, NOTE_AS4, NOTE_AS4, NOTE_AS4,
  NOTE_AS4, NOTE_G4, NOTE_C5, NOTE_AS4,
  NOTE_AS4, NOTE_A4, NOTE_A4, NOTE_C4,
  NOTE_F4, NOTE_F4, NOTE_F4, NOTE_G4,
  NOTE_A4, NOTE_A4, NOTE_A4, NOTE_A4,
  NOTE_G4, NOTE_A4, NOTE_AS4, NOTE_E4,
  NOTE_G4, NOTE_F4
};

int tree_tempo[] = {
  4,
  3, 8, 4, 4,
  3, 8, 3, 8,
  8, 8, 4, 4,
  4, 4, 4,
  3, 8, 4, 4,
  3, 8, 3, 8,
  8, 8, 4, 4,
  4, 4, 8, 8,
  8, 8, 3, 8,
  8, 8, 3, 8,
  8, 8, 3, 8,
  8, 8, 4, 4,
  3, 8, 4, 4,
  3, 8, 3, 8,
  8, 8, 4, 4,
  4, 4
};

// The First Noel (73 notes)
int noel_melody[] = {
  NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4,
  NOTE_G4, NOTE_A4, NOTE_B4,
  NOTE_C5, NOTE_B4, NOTE_A4,
  NOTE_G4, NOTE_A4, NOTE_B4,
  NOTE_C5, NOTE_B4, NOTE_A4,
  NOTE_G4, NOTE_A4, NOTE_B4,
  NOTE_C5, NOTE_G4, NOTE_F4,
  NOTE_E4,
  NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4,
  NOTE_G4, NOTE_A4, NOTE_B4,
  NOTE_C5, NOTE_B4, NOTE_A4,
  NOTE_G4, NOTE_A4, NOTE_B4,
  NOTE_C5, NOTE_B4, NOTE_A4,
  NOTE_G4, NOTE_A4, NOTE_B4,
  NOTE_C5, NOTE_G4, NOTE_F4,
  NOTE_E4,
  NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4,
  NOTE_G4, NOTE_C5, NOTE_B4,
  NOTE_A4, NOTE_A4,
  NOTE_G4, 1,
  NOTE_C5, NOTE_B4, NOTE_A4,
  NOTE_G4, NOTE_A4, NOTE_B4,
  NOTE_C5, NOTE_G4, NOTE_F4,
  NOTE_E4
};

int noel_tempo[] = {
  8, 8,
  3, 8, 8, 8,
  2, 8, 8,
  4, 4, 4,
  2, 8, 8,
  4, 4, 4,
  4, 4, 4,
  4, 4, 4,
  2,
  8, 8,
  3, 8, 8, 8,
  2, 8, 8,
  4, 4, 4,
  2, 8, 8,
  4, 4, 4,
  4, 4, 4,
  4, 4, 4,
  2,
  8, 8,
  3, 8, 8, 8,
  2, 8, 8,
  2, 4,
  2, 4,
  4, 4, 4,
  4, 4, 4,
  4, 4, 4,
  2
};

/* * * * * * * * * * * * * * Other Song Ideas * * * * * * * * * * * * * */

// Let It Snow

// Frosty the Snowman

// Sleigh Ride

// Jolly Old St. Nicholas

// Up On The Housetop

// Greensleeves (What Child Is This?)

// Winter Wonderland

// Silver Bells

// The Nutcracker: Dance of the Sugar Plum Fairy

// You're a Mean One Mr. Grinch

// Twelve Days of Christmas

// Jingle Bell Rock

// Feliz Navidad

// It's the Most Wonderful Time of the Year

// White Christmas

// A Holly Jolly Christmas

// Do You Hear What I Hear?

// Good King Wenceslas

// We Three Kings

// Away in a Manger

// Hark! The Herald Angels Sing

// O Holy Night

// O Come All Ye Faithful

// O Come O Come Emmanuel

// Silent Night

// Joy To The World
