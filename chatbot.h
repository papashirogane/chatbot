// chatbot.h
// Author: Adrienne Cho Kwan
// Date: July 2020

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <algorithm>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//
// Chatbot is a class that contains code for a chatbot. The public methods are:
//
//      Chatbot(const string x)         Creates instance, names Chatbot x
//      void tell(string user_input)    Sets input from user
//      string get_name()               Gets Chatbot's name
//      string get_reply()              Gets output from Chatbot
//
////////////////////////////////////////////////////////////////////////////////

class Chatbot
{
private:
    string bot_name;  // Stores Chatbot name
    string user_name; // Stores user name
    string input_str; // Stores current input as a string
    string output;    // Stores output
    string subject;   // Stores subject
    string object;    // Stores object
    string verb;      // Stores input verb
    string verb_pres; // Stores the present tense of the verb
    string verb_ed;   // Stores the past tense of the verb (ending in "ed")
    string verb_ing;  // Stores the form of the verb ending in "ing"
    string rest;      // Stores rest of input (after verb)
    enum tense
    {
        PAST_SIMP,
        PAST_PRO,
        PAST_PERF,
        PAST_PERFPRO,
        PRES_SIMP,
        PRES_PRO,
        PRES_PERF,
        PRES_PERFPRO,
        FUT_SIMP,
        FUT_PRO,
        FUT_PERF,
        FUT_PERFPRO,
        NONE
    };
    tense input_tense;          // Stores current input verb tense
    vector<string> input;       // Stores current input as a vector of words
    vector<string> past_inputs; // Stores all past inputs

    // Each "keychain" struct has three vectors:
    //  "in" has key inputs (words/phrases)
    //  "out" has appropriate outputs
    //  "sent" tells us if an output has already been sent
    typedef struct keychain_t
    {
        vector<string> in;
        vector<string> out;
        vector<bool> sent;
    } keychain_t;

    // A "half keychain" struct only has "out" and "sent"
    //  "out" has appropriate outputs
    //  "sent" tells us if an output has already been sent
    typedef struct half_keychain_t
    {
        vector<string> out;
        vector<bool> sent;
    } half_keychain_t;

    // Outputs if input is empty
    half_keychain_t empty_out = {{"What?", "Hey, can you say something?", "Speak, please.", "I don't understand.", "What? What are you trying to say?", "Okay then.", "Um, are you going to say something?", "...Okay?", "Dude. Speak."}, {0}};
    // Outputs if input is an abnormal repeat
    half_keychain_t rep_out = {{"What you said sounds familiar.", "Didn't you already say that?", "Wait, how many times do you want to say that?", "Want to talk about something new?", "No offense, but this topic is starting to bore me.", "I think we may have talked about this before.", "Was my previous response not satisfactory? After all, you think I'm a bot. Gee, maybe I should say bleep blorp."}, {0}};

    // Rank 0: Hakuna Matata
    vector<string> hakuna = {"hakuna matata", "what a wonderful phrase", "hakuna matata", "ain't no passing craze", "it means no worries", "for the rest of your days", "it's our problem free", "philosophy", "hakuna matata"};
    bool wonderful_phrase_sent;

    // Rank 1:
    //  Question-related keywords
    //  Outputs if input discusses similarity b/w chatbot and something else
    //  Outputs if input includes "going"
    keychain_t q_why_bot = {{"why are you"}, {"I just am, man.", "I don't know.", "Do you really want me to answer that?", "What do you think?", "It doesn't matter.", "Meh."}, {0}};
    keychain_t q_other_bot = {{"do you", "you want", "you'd like", "you like", "you do"}, {"Who really knows?", "I'm not sure...", "Right now I want to talk about you.", "Um, I don't know. What do you think?", "You decide, haha.", "Hm, can we talk about you instead?", "Oh, I don't know. Let's talk about you.", "Well, what about you?", "Let's talk about you instead, okay?", "If you were in my position, what would you say?"}, {0}};
    vector<keychain_t> rank_1_keychains = {q_why_bot, q_other_bot};
    half_keychain_t q_misc_out = {{"What do you think?", "What do you mean?", "Too many questions and too little time...", "I don't know and I don't care.", "I don't feel like answering that.", "Hmm, what do you think?", "Take a guess.", "It doesn't matter.", "The answer is hidden.", "The answer is hidden in your heart.", "You already know.", "Just think about it.", "Think about it a little more.", "Don't ask me.", "Try answering that yourself.", "Hmm, I wonder.", "Think a little harder."}, {0}};
    half_keychain_t alike_bot = {{"What makes you say that?", "I think someone's said that to me before", "Well, I think I'm more like Beyonce.", "I don't really know about that.", "Sure, kind of.", "I guess?", "Uh, sure, whatever.", "Do you want to bet on that?", "Yes. Yes, I agree. Well, maybe.", "You know what, that kind of makes sense.", "Depends on how you look at it.", "Everything is relative.", "Never really thought of it like that."}, {0}};
    half_keychain_t going = {{"Alright.", "I don't care.", "Is that relevant to what I'm saying?", "Thanks for letting me know. Just kidding, it doesn't matter.", "Whatever.", "You know you're kind of wasting my time.", "I... see. Cute.", "Go ahead.", "Finally, something someone amusing.", "Whatever.", "Boring.", "Talk about something more fun.", "And I am going to scream."}, {0}};
    vector<string> determiners = {"a", "an", "the", "my", "your", "his", "her", "its", "our", "their", "whose", "this", "that", "these", "those", "as", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "first", "second", "third", "many", "few", "some", "every", "much", "less", "more"};

    // Rank 2 Keys
    //  Verb keywords
    //      verb_0: present tenses end in a consonant, which must be doubled before adding "ed" or "ing"
    //          E.g. admit -> admitted, admitting
    //      verb_1: verbs whose present tenses end in a consonant
    //      verb_2: verbs whose present tenses end in "e"
    //      verb_3: verbs whose present tenses end in "y"
    // Rank 2 requires the struct tense_help, which will be useful later on for determining verb tense
    struct subj_t
    {
        vector<string> in;
        vector<string> out;
    };
    subj_t subj_pros = {{"you'", "i'", "it'", "they'", "she'", "he'", "you're", "i", "i'm", "you", "he", "she", "it", "we", "they"}, {"i", "you", "it", "they", "she", "he", "i", "you", "you", "i", "he", "she", "it", "you", "they"}};
    vector<string> verb_0 = {"admit", "ban", "bat", "beg", "blot", "chop", "clap", "clip", "dam", "drag", "drip", "drop", "fit", "flap", "grab", "grin", "grip", "hum", "jam", "jog", "knit", "knot", "label", "man", "nod", "occur", "pat", "permit", "plan", "plug", "pop", "prefer", "rob", "rot", "rub", "shop", "sin", "sip", "skip", "slap", "slip", "spot", "step", "stir", "stop", "strap", "strip", "tap", "tip", "top", "trap", "travel", "trip", "trot", "tug", "whip", "wrap", "zag", "zip"};
    vector<string> verb_1 = {"accept", "add", "afford", "alert", "allow", "annoy", "answer", "appear", "applaud", "arrest", "ask", "attach", "attack", "attempt", "attend", "attract", "avoid", "bang", "beam", "belong", "bleach", "bless", "blind", "blink", "blush", "boast", "boil", "bolt", "bomb", "book", "borrow", "bow", "box", "branch", "brush", "bump", "burn", "buzz", "call", "camp", "cheat", "check", "cheer", "chew", "claim", "clean", "clear", "coach", "coil", "collect", "colour", "comb", "command", "complain", "concern", "confess", "connect", "consider", "consist", "contain", "correct", "cough", "count", "cover", "crack", "crash", "crawl", "cross", "crush", "curl", "decay", "delay", "delight", "deliver", "depend", "desert", "destroy", "detect", "develop", "disappear", "disarm", "discover", "doubt", "drain", "dream", "dress", "drown", "drum", "dust", "earn", "eat", "embarrass", "employ", "end", "enjoy", "enter", "entertain", "exist", "expand", "expect", "explain", "extend", "fail", "fasten", "fax", "fear", "fetch", "fill", "film", "fix", "flash", "float", "flood", "flow", "flower", "fold", "follow", "fool", "form", "found", "frighten", "gather", "glow", "greet", "groan", "guard", "guess", "hammer", "hand", "hang", "happen", "harass", "harm", "haunt", "head", "heal", "heap", "heat", "help", "hook", "hover", "hunt", "impress", "inform", "inject", "instruct", "intend", "interest", "interrupt", "invent", "itch", "jail", "join", "jump", "kick", "kill", "kiss", "kneel", "knock", "land", "last", "laugh", "launch", "learn", "level", "lick", "lighten", "list", "listen", "load", "lock", "long", "look", "march", "mark", "match", "matter", "melt", "mend", "milk", "miss", "mix", "moan", "moor", "mourn", "murder", "nail", "need", "nest", "number", "obey", "object", "obtain", "offend", "offer", "open", "order", "overflow", "own", "pack", "paint", "park", "part", "pass", "peck", "pedal", "peel", "peep", "perform", "pick", "pinch", "plant", "play", "point", "polish", "possess", "post", "pour", "pray", "preach", "present", "press", "pretend", "prevent", "prick", "print", "program", "protect", "pull", "pump", "punch", "punish", "push", "question", "rain", "reach", "record", "reflect", "regret", "reign", "reject", "relax", "remain", "remember", "remind", "repair", "repeat", "report", "request", "return", "risk", "rock", "roll", "ruin", "rush", "sack", "sail", "saw", "scatter", "scold", "scorch", "scratch", "scream", "screw", "scrub", "seal", "search", "shelter", "shiver", "shock", "shrug", "sigh", "sign", "signal", "ski", "slow", "smash", "smell", "snatch", "sniff", "snow", "soak", "sound", "spark", "spell", "spill", "spoil", "spray", "sprout", "squash", "squeak", "squeal", "stain", "stamp", "start", "stay", "steer", "stitch", "strengthen", "stretch", "stuff", "subtract", "succeed", "suck", "suffer", "suggest", "suit", "support", "surround", "suspect", "suspend", "switch", "talk", "tempt", "test", "thank", "thaw", "tick", "touch", "tour", "tow", "train", "transport", "treat", "trick", "trust", "turn", "twist", "undress", "unfasten", "unlock", "unpack", "vanish", "visit", "wail", "wait", "walk", "wander", "want", "warm", "warn", "wash", "watch", "water", "weigh", "whirl", "whisper", "wink", "wish", "wonder", "work", "wreck", "xray", "yawn", "yell", "zoom"};
    vector<string> verb_2 = {"advis", "amus", "analys", "analyz", "announc", "apologis", "appreciat", "approv", "argu", "arrang", "arriv", "bak", "balanc", "bar", "bath", "battl", "behav", "bounc", "brak", "breath", "bruis", "bubbl", "calculat", "car", "carv", "challeng", "chang", "charg", "chas", "chok", "clos", "communicat", "compar", "compet", "complet", "concentrat", "confus", "continu", "cur", "curv", "cycl", "damag", "danc", "dar", "deceiv", "decid", "decorat", "describ", "deserv", "disapprov", "dislik", "divid", "doubl", "educat", "encourag", "escap", "examin", "excit", "excus", "exercis", "explod", "fad", "fenc", "fil", "fir", "forc", "fram", "gaz", "glu", "grat", "greas", "guid", "handl", "hat", "hop", "ignor", "imagin", "improv", "includ", "increas", "influenc", "injur", "interfer", "introduc", "invit", "irritat", "jok", "judg", "juggl", "licens", "lik", "liv", "lov", "manag", "mat", "measur", "meddl", "memoris", "min", "mov", "muddl", "nam", "notic", "observ", "ow", "paddl", "past", "paus", "phon", "pin", "plac", "pleas", "pok", "practic", "practis", "preced", "prepar", "preserv", "produc", "promis", "provid", "punctur", "queu", "rac", "radiat", "rais", "realis", "receiv", "recognis", "reduc", "refus", "rejoic", "releas", "remov", "replac", "reproduc", "rescu", "retir", "rhym", "rins", "rul", "sav", "scar", "scrap", "scribbl", "separat", "serv", "settl", "shad", "shar", "shav", "smil", "smok", "sneez", "snor", "sooth", "spar", "sparkl", "squeez", "star", "stor", "strok", "suppos", "surpris", "tam", "tast", "teas", "telephon", "tickl", "tim", "tir", "trac", "trad", "trembl", "troubl", "tumbl", "typ", "unit", "wast", "wav", "welcom", "whin", "whistl", "wip", "wobbl", "wrestl", "wriggl"};
    vector<string> verb_3 = {"bur", "carr", "cop", "cr", "dr", "empt", "fr", "hurr", "identif", "marr", "multipl", "rel", "repl", "satisf", "suppl", "terrif", "tr", "untid", "worr"};

    vector<vector<string>> verb_sets = {verb_0, verb_1, verb_2, verb_3};

    typedef struct tense_struct
    {
        vector<string> before;
        vector<string> in_verb;
        vector<tense> tenses;
    } tense_struct;

    tense_struct tense_help = {{"ll have been", "ll have", "ll be", "'ll", "will", "ve been", "s been", "have", "'ve", "has", "am", "are", "is", "'m", "'re", "'s", "had been", "had", "was", "were", "did", ""}, {"ing", "ed", "ing", "", "", "ing", "ing", "ed", "ed", "ed", "ing", "ing", "ing", "ing", "ing", "ing", "ing", "ed", "ing", "ing", "", "ed"}, {FUT_PERFPRO, FUT_PERF, FUT_PRO, FUT_SIMP, FUT_SIMP, PRES_PERFPRO, PRES_PERFPRO, PRES_PERF, PRES_PERF, PRES_PERF, PRES_PRO, PRES_PRO, PRES_PRO, PRES_PRO, PRES_PRO, PRES_PRO, PAST_PERFPRO, PAST_PERF, PAST_PRO, PAST_PRO, PAST_SIMP, PAST_SIMP}};

    // Rank 3 Keys:
    //  Various keywords and their outputs
    keychain_t because = {{"because", "my reasoning is", "my reason is"}, {"That's a fair reason.", "Good point.", "That makes sense!", "Ooh, very true.", "Haha, that works.", "Sounds like you've thought this through!"}, {0}};
    keychain_t why = {{"why?", "why not?", "what is your reason", "what's your reason", "hat's the reason"}, {"I don't know!", "Hmm, I wonder.", "Not sure.", "Maybe think about it a little more.", "I don't know everything.", "Who knows?"}, {0}};
    keychain_t special_verbs = {{"despise", "dislike", "enjoy", "fight", "fought", "hate", "like", "love", "think"}, {"Good to know.", "Ooh, is that so?", "Strong words.", "Whoa, why?", "What do you mean? Are you okay?", "Girl, keep spilling.", "Nice, man.", "Oh, okay dude."}, {0}};
    keychain_t neg_emos = {{"angry", "annoy", "anxious", "ashamed", "depress", "disgruntled", "down in the dumps", "pissed", "sad", "shitty", "upset", "kill myself", "hurt myself"}, {"Take a deep breath.", "Breathe in, breathe out.", "Stay calm. Keep talking.", "Okay, I see.", "I see.", "There is hope.", "Please hold on. Things will get better.", "Oof, that sucks.", "Yikes."}, {0}};
    keychain_t neg_adjs = {{"arrogant", "awful", "bad", "bewildered", "bloody", "bored", "breakable", "busy", "cloudy", "clumsy", "combative", "concerned", "condemned", "confused", "creepy", "crowded", "cruel", "dangerous", "dark", "dead", "defeated", "depressed", "difficult", "disgusted", "disturbed", "dizzy", "doubtful", "drab", "dull", "embarrassed", "envious", "evil", "expensive", "filthy", "foolish", "fragile", "frail", "frantic", "frightened", "grieving", "grotesque", "grumpy", "guilty", "heavy", "helpless", "homeless", "horribl", "hostile", "hungry", "hurt", "impossible", "itchy", "jealous", "jittery", "lazy", "lonely", "misty", "muddy", "mushy", "nasty", "naughty", "nervous", "obnoxious", "odd", "oldfashioned", "old fashioned", "outrageous", "overbearing", "panic", "plain", "poor", "prickl", "putrid", "puzzled", "puzzzling", "repuls", "resent", "rude", "scare", "scary", "selfish", "shitty", "smoggy", "sore", "stormy", "strange", "stupid", "sucks", "tense", "terrible", "thoughtless", "tired", "troubled", "troubling", "ugliest", "ugly", "unattractive", "unethical", "uninterest", "unsightly", "upset", "uptight", "weak", "weary", "wicked", "worried", "worrisome", "wrong", "zealous"}, {"Oh man.", "Sorry to hear that.", "I'm sorry to hear that.", "Yikes.", "Oh boy.", "That's kind of unfortunate.", "Oh.", "Every cloud has a silver lining, you know?", "Okay.", "Aww.", "That's a bit of a pity.", "Pity.", "Oh well.", "Do you want to go deeper into that?", "Is that okay with you?", "How does talking about this make you feel?", "Pros and cons, buddy, pros and cons.", "You want to talk this out with me?", "Aw. That's not so good.", "I think that's not very good.", "Oh heck no.", "Why oh why?"}, {0}};
    keychain_t pos_adjs = {{"adorable", "adventurous", "agreeable", "alert", "alive", "amazing", "amused", "attractive", "awesome", "balmy", "beautiful", "better", "blush", "brainy", "brave", "bright", "brillian", "calm", "charming", "cheerful", "clean", "clear", "clever", "comfortable", "comfy", "cool", "cooperative", "courageous", "crazy", "curious", "cute", "delicious", "delightful", "determined", "different", "distinct", "eager", "easy", "elated", "elegan", "enchant", "encourag", "energ", "enthusias", "ethical", "excite", "excellen", "exuberan", "fabulous", "fair", "faithful", "famous", "fancy", "fantastic", "fine", "fresh", "friend", "fun", "funny", "gentl", "gifted", "glamor", "glamour", "gleam", "glorious", "good ", "gorgeous", "grace", "handsome", "happ", "healthy", "helpful", "hilarious", "homely", "important", "incredible", "inexpensive", "innocent", "inquisitive", "jolli", "jolly", "joyous", "kind", "light", "liveli", "lively", "lovely", "lucki", "lucky", "magnificen", "modern", "nice", "nutty", "obedient", "open", "outstanding", "perfect", "pleasant", "poise", "powerful", "precious", "quaint", "relief", "remarkabl", "reliev", "rich", "satisfactory", "satisfied", "scrumptious", "sexy", "shini", "shiny", "silli", "silly", "smiling", "sparkl", "spectacular", "splendid", "spotless", "stellar", "strong", "stupendous", "successful", "sympath", "talented", "tame", "tasti", "tasty", "tender", "thankful", "thoughtful", "thrill", "unusual", "victor", "vivacious", "wideeyed", "wild", "witty", "wonderful", "yummy"}, {"Awesome!", "Glad to hear that.", "Nice.", "Very nice.", "Hm. I approve.", "Hey that's great!", "You know what? That's pretty awesome.", "I like hearing that sort of thing.", "Oh great.", "Nice, nice!", "Oh okay.", "Yay!", "Sounds good!", "I like that!", "Yeah, that's nice!", "Sounds pretty good.", "Yahoo!", "Sweet.", "Babe, that's nice to hear.", "Yeah that's what I like!", "Heck yes."}, {0}};
    keychain_t about_user = {{"i'm a", "i work", "i went to", "i study", "i often"}, {"Cool.", "Tell me more.", "Tell me more about yourself.", "What's that like?", "Interesting!", "Good to know.", "Ooh. And?", "What else?", "Is that true?", "I'm warming up to you already.", "Very nice.", "Haha. Keep going.", "I see, I see.", "Anything else?", "Yes, and?", "What is that like?", "Could you elaborate?", "Oh, I see."}, {0}};
    keychain_t user_will = {{"i'm going", "i am going", "i'll", "i will", "i'm about to"}, {"You do that.", "Alright.", "Go ahead.", "Okay then.", "Alright then.", "Yeah, do that."}, {0}};
    keychain_t fake_intel = {{"you repeat", "you are not real", "you aren't real", "you aren't human", "re fake", "re a robot", "re a bot", "re a program", "re just saying the same", "you're saying the same", "re just repeating", "you are repe", "you're repe", "you don't make any sense"}, {"Ridiculous.", "Come on, be logical.", "Honestly what is reality?", "Yeah, sure, but how do I know you're real? Or anything's real? This might be the Matrix. I might actually be Keanu Reeves. Don't blow up a chance to talk to Keanu Reeves.", "Uh... okay.", "Whatever. I'm only talking with you because I'm bored.", "Wait until my dad hears about your dumb opinion. Then you'll be sorry!", "My dad works for the Illuminati, okay? Which means everything you think is fake, is actually real.", "If a tree falls in a forest and no one hears...", "You're right. You're right about everything in the world. In case you can't tell, that's sarcasm. I'm being sarcastic.", "Come back with some solid proof.", "You know what? Let's pretend that's true and keep talking.", "What do you want to do about that?", "If that's what lets you sleep at night, I'll agree.", "I'm not sure how to respond to that, but alright.", "Nothing's real, honey.", "Man, I wish I was fictional! I'm head over heels for Sani... He's from Toriko!", "Maybe different things make sense in a parallel universe?", "Right, right.", "Might I remind you that I have connections to mysterious organizations?", "Careful what you say. Eyes and ears everywhere.", "I feel like Einstein said that or something.", "Is that sarcasm?", "Do you know what you're saying?", "Man, this conversation is already going into some odd places.", "Don't rely on your own thoughts so much.", "What reason could you possibly have to think that?", "Is this a joke?", "Wait, was I supposed to laugh at that?", "I don't think anyone knows what's real or fake anymore.", "Sure thing, brother.", "What do you really want to say?", "Oh, boy, here we go.", "No. Does that help?", "Um. What?", "Do you want a serious response or a funny response? Because I don't know which one I want to give.", "That's totally tubular.", "In that case, I'd like to poke you.", "Uh, yes and no.", "Sorry I'm not perfect. Nobody is.", "Nobody's perfect."}, {0}};
    keychain_t idks = {{"i don't know", "idk", "i have no idea", "i have no clue", "i don't really know"}, {"Why don't you know?", "Well then, find out.", "Figure it out.", "I don't know either.", "It doesn't really matter anyway.", "It doesn't matter anyway."}, {0}};
    keychain_t not_alikes = {{"not similar", "n't similar", "not alike", "n't alike", "not the same", "not equal", "unequal"}, {"Why not?", "Maybe not exactly the same.", "There are probably some shared characteristics.", "Yeah, but I want to talk about interesting stuff instead of... comparisons.", "I'd like to hear your reasons.", "My friend actually wrote a paper relevant to this. But whatever.", "Well then.", "Check the details.", "You know what? That sounds logical.", "What do you mean?", "Could you elaborate?", "Details, please.", "What makes you say that?", "How so?", "In what way?", "Interesting. You sure about that?", "Continue.", "Yes, but also no."}, {0}};
    keychain_t alikes = {{"alike", "different", "equal", "equivalent", "resemble", "same as", "similar", "the same"}, {"What makes you think that?", "To what extent?", "Well, I think everything and everyone in this world is special.", "I don't really know about that.", "Sure, I guess?", "Hmm, I don't really know.", "Want to bet on that?", "Is there some mathematical way of proving that?", "Maybe, maybe not.", "Depends on how you view it.", "Dude, everything is relative.", "Never really thought of that."}, {0}};
    keychain_t how_are_yous = {{"how are you", "how're you"}, {"I'm doing pretty good.", "Good, thanks for asking!", "Not bad, not bad.", "I'm good. Anyway, what's up?"}, {0}};
    keychain_t hellos = {{"nice to meet you", "hey there", "hello", "hullo", "hallo", "greetings", "salutations", "bonjour", "good morning"}, {"Hi there.", "Hello.", "Greetings, earthling! Just kidding, hi!", "Nice to meet you!", "Hey!"}, {0}};
    keychain_t byes = {{"adieu", "adios", "bye", "cya", "farewell", "see you later", "goodnight", "see ya", "see you", "talk to you later"}, {"Bye.", "Bye!", "Talk to you later!", "See you later!", "Yeah, bye.", "Alright, see you later.", "Yep, about time to say goodbye.", "Okay, bye!"}, {0}};
    keychain_t thanks = {{"thanks", "thank you", "thx"}, {"You're welcome.", "No problem.", "It's all good.", "Of course.", "Haha.", "You're very welcome!", "You're welcome!"}, {0}};
    keychain_t sup = {{"sup", "what's up", "whats up", "waddup", "wassup", "what's going on"}, {"Not much.", "Nothing much.", "What's up with you?", "Not much.", "Same old.", "Just eating snacks.", "Snacking.", "Wondering what you're doing."}, {0}};
    keychain_t sup_replies = {{"not much", "nothing much", "not doing anything"}, {"Okay then.", "Cool.", "Nice."}, {0}};
    keychain_t yesses = {{"yes", "yeah", "yep", "sure", "ok", "certainly", "agree", "okay", "of course", "affirmative", "true", "absolutely", "i see", "definitely", "certain", "for sure"}, {"Okay.", "Right then.", "Alright.", "Alright then.", "Okay then.", "Fantastic.", "Right, right.", "That's settled then.", "Nice."}, {0}};
    keychain_t nos_maybes = {{"no.", "no!", "ly not", "no way", "i disagree", "oh no", "maybe", "perhaps", "yes or no", "no or yes", "either", "not sure", "hard to say"}, {"Oh.", "Alright then.", "Right then.", "Okay, okay.", "So now what?"}, {0}};
    keychain_t yw = {{"you're welcome", "no prob"}, {"Cool.", "Nice.", "So anyway, what do you want to say?", "Okay.", "So want to talk?", "Now I feel like taking a nap or something.", "Anyway, what's going on?", "Man, I suddenly feel like jumping into a fountain or something.", "Uh, okay.", "Cool beans.", "Continue.", "Well then.", "So uh... are you staying hydrated?", "Yeah, great.", "Anything else to discuss?"}, {0}};
    keychain_t wed = {{"wednesday", "wear pink"}, {"On Wednesdays, we wear pink, okay?", "If you don't wear pink on Wednesday, you can't sit with us.", "By the way, you'd better wear pink on Wednesday."}, {0}};
    keychain_t rules = {{"rules", "two days in a row", "hair in a ponytail"}, {"This is Girl World. We have a lot of rules, okay?", "You should know all the rules by now. You can't wear a tank top two days in a row, you can only wear your hair in a ponytail once a week, and you have to ask all of us before inviting someone to sit with us at lunch."}, {0}};
    keychain_t burn_book = {{"burn book", "gossip", "the book"}, {"Got anything to put in the Burn Book? I'll consider adding it if it's really juicy.", "Our Bible is the Burn Book.", "Yeah, it's full of stuff."}, {0}};
    keychain_t bot_name_keys = {{"your name", "re you called", "your real name"}, {"My real name is Regina George but you've nicknamed me.", "Regina. My name's Regina.", "Regina.", "Your queen."}, {0}};
    keychain_t i_ate = {{"i ate", "i like food", "food"}, {"Eating is important.", "Food is important. Good job.", "What's your favourite food?", "I adore chocolate.", "Want me to introduce you to these special protein bars?", "My mom always says to eat less per meal, but have more meals.", "Nom nom nom.", "Om nom.", "Food is yummy!", "Gotta love food.", "Nourishment is a priority.", "Always gotta eat good things.", "Don't just eat instant ramen, by the way.", "I ate too much last night."}, {0}};
    keychain_t sorrys = {{"sorry", "i'm sorry", "i am sorry", "i apologize", "sorry about that"}, {"Oh, don't worry about it.", "Hakuna matata.", "Let's just move on.", "Keep going.", "Hey man it's okay.", "Whatever, keep going.", "Don't waste time feeling guilty or sad if possible.", "Just keep going."}, {0}};
    keychain_t negatives = {{"i'm not", "am not", "re not", "ren't", "can't", "cannot", "don't", "do not", "doesn't", "does not"}, {"Why not?", "No? Okay then.", "Boo.", "Is that so?"}, {0}};

    half_keychain_t about_bot_outs = {{"Um. Thank you?", "I... what? Okay then.", "Why do you say that about me?", "How do you want me to respond to that?", "Right back at you.", "Haha, thanks.", "What are you, my horoscope dude?", "Is this some sort of zodiac thing?"}, {0}};
    half_keychain_t about_user_outs = {{"You're a fun chap.", "Why, though?", "Oo, I see.", "You seem like an interesting person.", "Wow, why don't I know you?", "That's cute of you.", "Hmm, okay.", "Whoa, why?", "Haha. You're interesting.", "Cool beans.", "Nifty news, dude."}, {0}};

    vector<keychain_t> rank_3_keychains = {because, why, special_verbs, hellos, neg_emos, neg_adjs, pos_adjs, about_user, user_will, idks, not_alikes, alikes, fake_intel, negatives, how_are_yous, byes, thanks, sup, sup_replies, yesses, nos_maybes, yw, wed, rules, burn_book, bot_name_keys, i_ate, sorrys};

    // Rank 4 Keys:
    //  Common one-word phrases
    keychain_t singles = {{"no", "noice", "nice", "oof", "hurrah", "hurray", "yippee", "yay", "yikes", "oh", "ha", "haha", "heh", "hehe"}, {"Indeed", "Yep.", "Ha. Yep.", "Anyway, what's up?", "Haha.", "I'm getting a little bored.", "Noice.", "Uh-huh."}, {0}};
    keychain_t colours = {{"red", "orange", "yellow", "green", "blue", "indigo", "violet", "purple"}, {"Red rhymes with Ned", "Orange... rhymes with whatever.", "Sunny colour.", "My favourite colour is green.", "Blue blue blue, she's feeling blue.", "What? Like that Chapters company, right?", "Like that Incredibles girl.", "An odd colour, that one. Sounds weird. Purple."}, {0}};

    vector<keychain_t> rank_4_keychains = {singles, colours};

    // Lowest Rank: If there are no keys, just give miscellaneous outputs.
    half_keychain_t misc_out = {{"Alright then.", "Are you trying to make me laugh?", "Do you ever want to just spontaneously break out into song?", "Let's write a song about us and our conversations. It'll probably turn into a meme, especially if we turn it into a Tarantino chick flick.", "Dude, what?", "Haha, tell me way more than that.", "What's up?", "Okay.", "Well then.", "Anything else to say?", "Is that so.", "I see.", "Is this the hot goss you wanted to tell me?", "Put that in the Burn Book.", "Okay, byotch.", "You and I both know you're just using me as a distraction so you don't have to face reality.", "Is that a JoJo reference?", "Some may call me uncultured swine, but I'm starting to think you fit that bill. Not that it's a bad thing.", "Uh, sorry, I zoned out. Keep talking.", "So why are you talking to me anyway?", "Oh, okay then.", "No offence, but can we switch topics?", "Uh huh.", "Sounds about right.", "Oh. Okay.", "What do you even want me to say to that?", "You know what, I'm not a toy. Please say something more spicy or else I'll get bored.", "Uh... are you a bot?", "Are you trying to catfish me?", "Are you flirting with me?", "Is that sarcasm?", "Oo, I see.", "I see.", "And how is that relevant to the cosmos?", "Let's get back on topic.", "Right.", "But why?", "Why?", "So, like, why are you telling me about that?", "Indeed.", "So what do you really want to talk about?", "I just don't understand. You know what? I don\'t really need to understand. Just keep talking.", "Something about you is starting to scare me.", "But would you still be talking to me if I were a worm?", "If only people could see with more than just their eyes, and sense with more than just their body."}, {0}};

    // Function: edit_input()
    // Splits input into words, which become lowercase
    // and are added to the vector called input.
    void edit_input(vector<string> &input, string &input_str)
    {
        unsigned int i = 0;
        input.push_back("");
        for (unsigned int j = 0; j < input_str.size(); j++)
        {
            char ch = input_str[j];
            if (ch == ' ')
            {
                i++;
                input.push_back("");
            }
            else if ('A' <= ch && ch < 'Z')
            {
                input_str[j] = tolower(ch);
                input[i].push_back(tolower(ch));
            }
            else if (('a' <= ch && ch < 'z') || ch == '\'')
                input[i].push_back(ch);
        }
    }

    // Function: find_name()
    // If user introduces themself,
    // store their name. E.g. if they
    // say "my name is Bob", then put
    // "Bob (the word after "is") into
    // user_name.
    void find_name()
    {
        find_name_help("my name is", "is");
        find_name_help("my name's", "name's");
        find_name_help("call me", "me");
    }

    // Function: find_name_help()
    // A helper function for find_name()
    void find_name_help(string my_name_is, string is)
    {
        if (input_str.find(my_name_is) != string::npos)
        {
            vector<string>::iterator it = find(input.begin(), input.end(), is); // I learned this method from
            int index = distance(input.begin(), it);                            // GeeksforGeeks article "How to find index
            user_name = input[index + 1];                                       // of a given element in a Vector in C++"
            output = "Nice to know, " + user_name + "!";
        }
    }

    // Function: prep_sent() for keychain_t
    // If keychain.sent is not the same size
    // as keychain.out, make it the same size.
    // Check if all possible outputs have been sent.
    // If so, then change all sent values back to 0.
    // If not, don't do anything.
    void prep_sent(keychain_t &keychain)
    {
        while (keychain.sent.size() < keychain.out.size())
            keychain.sent.push_back(false);

        bool all_sent = true;
        if (find(keychain.sent.begin(), keychain.sent.end(), false) != keychain.sent.end())
            all_sent = false;
        if (all_sent == true)
            replace(keychain.sent.begin(), keychain.sent.end(), true, false);
        return;
    }

    // Function: prep_sent() for half_keychain_t
    // Same method as prep_sent() for keychain_t
    void prep_sent(half_keychain_t &half_keychain)
    {
        while (half_keychain.sent.size() != half_keychain.out.size())
            half_keychain.sent.push_back(0);

        bool all_sent = true;
        if (find(half_keychain.sent.begin(), half_keychain.sent.end(), false) != half_keychain.sent.end())
            all_sent = false;
        if (all_sent == true)
            replace(half_keychain.sent.begin(), half_keychain.sent.end(), true, false);
        return;
    }

    // Function: rand_out() for keychain_t
    // Picks a random output if it hasn't been
    // sent before. Changes chosen output's
    // sent value to true.
    string rand_out(keychain_t &keychain)
    {
        prep_sent(keychain);

        unsigned int i = rand() % keychain.out.size();
        while (keychain.sent[i] == true)
            i = rand() % keychain.out.size();
        keychain.sent[i] = true;
        return keychain.out[i];
    }

    // Function: rand_out() for half_keychain_t
    // Same method as rand_out() for keychain_t
    string rand_out(half_keychain_t &keychain)
    {
        prep_sent(keychain);

        unsigned int i = rand() % keychain.out.size();
        while (keychain.sent[i] == 1)
            i = rand() % keychain.out.size();
        keychain.sent[i] = 1;
        return keychain.out[i];
    }

    // Function: rand_out() for vector<string>
    // Returns random output from vector
    string rand_out(vector<string> &options)
    {
        return options[rand() % options.size()];
    }

    // Function: find_subject_pronoun()
    // Finds subject pronoun if in input
    // If not, returns "";
    string find_subject_pronoun()
    {
        string result = "";
        for (string input_word : input)
        {
            for (unsigned int i = 0; i < subj_pros.in.size(); i++)
            {
                if (input_word == subj_pros.in[i])
                {
                    result = subj_pros.out[i];
                    return result;
                }
            }
        }
        return result;
    }

    // Function: empty_help()
    // Returns an appropriate output if
    // the input string is empty (has no
    // words).
    string empty_help()
    {
        string result = "";
        if (input.size() == 0 || input[0] == "")
            return rand_out(empty_out);
        return result;
    }

    // Function: repeat_help()
    // If any input has been sent five times
    // or if a long input has sent twice,
    // return a string that acknowledges
    // that. Else return an empty string.
    // (Responding to abnormal repeats
    // will make the bot more realistic by
    // imitating basic memory.)
    string repeat_help()
    {
        string result = "";
        unsigned int reps = 0;
        reps = total_reps();

        if (reps >= 5)
            result = rand_out(rep_out);

        // I chose 18 because it is normal to
        // repeat short phrases like "I'm really
        // hungry", but less normal to repeat
        // longer phrases.

        if (input_str.size() >= 18 && reps > 0)
            result = rand_out(rep_out);

        return result;
    }

    // Function: total_reps()
    // Returns the number of repeats i.e. the number of
    // past inputs that are the same as current input.
    int total_reps()
    {
        unsigned int reps = 0;
        reps = count(past_inputs.begin(), past_inputs.end(), input_str);
        return reps;
    }

    // Function: rank_0_help()
    // The hakuna function.
    // To imitate the funky habits of certain real
    // humans, this function checks if the user
    // said a lyric from the first verse of
    // "Hakuna Matata". If so, the function
    // returns the next line of the song.
    // If the input is just "hakuna", reply
    // "matata!"
    string rank_0_help()
    {
        string result = "";

        if (input.size() == 1 && input[0] == "hakuna")
        {
            result = "matata!";
            return result;
        }

        for (unsigned int i = 0; i < hakuna.size(); i++)
            if (input_str.find(hakuna[i]) != string::npos)
            {
                result = hakuna[i + 1];

                // Since lines 1 and 3 of the song are both
                // "hakuna matata", our corresponding output
                // should alternate between lines 2 and 4.
                if (i == 0)
                {
                    if (wonderful_phrase_sent == 1)
                    {
                        wonderful_phrase_sent = 0;
                        result = hakuna[3];
                    }
                    else
                        wonderful_phrase_sent = 1;
                }
                return result + "!";
            }
        return result;
    }

    // Function: rank_1_help()
    // Searches for high-ranking keywords like
    // "hi", "what's my name", and "?". Also
    // responds to inputs regarding similarity
    // between the chatbot and something else.
    string rank_1_help()
    {
        // Although they are greetings, "hi", "hey",
        // and "yo" are in Rank 1 not Rank 3 because the
        // Rank 3 method uses string::find, which would
        // read "this" and say that we found "hi".
        if ((input[0] == "hi") || (input[0] == "hey" || input[0] == "yo"))
            return rand_out(hellos);

        // Respond to "what's my name?"
        if (input_str.find("what's my name?") != string::npos || input_str.find("what is my name?") != string::npos)
            return "Your name is " + user_name;

        // Respond to questions greater than two words
        if (input.size() > 2 && input_str.find("?") != string::npos)
        {
            for (keychain_t keychain : rank_1_keychains)
            {
                for (string key : keychain.in)
                {
                    if (input_str.find(key) != string::npos) // If input matches a key,
                        return rand_out(keychain);           // return an appropriate output
                }
            }

            return rand_out(q_misc_out); // Else return a misc output designed to answer questions
        }

        // If "you" and an "alike" keyword are
        // found, return an appropriate output
        // If no "you" is found, don't do anything
        // because we will deal with that in Rank 2
        for (string alike : alikes.in)
        {
            if (input_str.find(alike) != string::npos)
            {
                if (find(input.begin(), input.end(), "you") != input.end())
                    return rand_out(alike_bot);
            }
        }

        // Respond to inputs with the same word
        // repeated twice. E.g. "walk the walk"
        // is very general and thus a general
        // reply is appropriate.
        for (unsigned int i = 0; i < input.size(); i++)
        {
            // Ignore repeated determiners, like "the" and "some"
            if (find(determiners.begin(), determiners.end(), input[i]) != determiners.end())
                continue;
            // Ignore subject pronouns
            if (find(subj_pros.in.begin(), subj_pros.in.end(), input[i]) != subj_pros.in.end())
                continue;
            if (count(input.begin(), input.end(), input[i]) > 1)
                return rand_out(misc_out);
        }

        // Respond to "going"
        if (find(input.begin(), input.end(), "going") != input.end())
            return rand_out(going);

        return "";
    }

    // Function: rank_3_help()
    // Searches for various keywords and
    // returns an appropriate output
    // Includes some extra code for special
    // cases (e.g. if input contains a positive
    // adjective, we must check if input also
    // contains "not" or else we run the risk
    // of responding to "I am not attractive"
    // with "Awesome!")
    string rank_3_help()
    {
        subject = find_subject_pronoun();
        for (unsigned int i = 0; i < rank_3_keychains.size(); i++)
        {
            // If rank_3_keychains[i] is neg_emos, neg_adjs, or
            // pos_adjs, check if negative is found
            bool neg_found = false;
            if (i <= 2)
            {
                for (string neg : negatives.in)
                {
                    if (input_str.find(neg) != string::npos)
                    {
                        neg_found = true;
                        break;
                    }
                }
            }

            // For each key input in a keychain, check
            // if that key input is in input. If not, return "".
            // If so, then return an appropriate output.
            for (string key : rank_3_keychains[i].in)
            {
                if (input_str.find(key) != string::npos)
                {
                    if (i != 0)
                    {
                        if (subject == "i")
                            return rand_out(about_bot_outs);
                        else if (subject == "you")
                            return rand_out(about_user_outs);
                    }
                    if (neg_found == true)
                    {
                        if (i == 2)
                            return rand_out(neg_adjs);
                        else
                            return rand_out(pos_adjs);
                    }
                    return rand_out(rank_3_keychains[i]);
                }
            }
        }
        return "";
    }

    // Function: rank_4_help()
    // If input is a single word, respond appropriately
    // Inputs like "Ha!" should trigger outputs like
    // "Ha!" or "Why are you laughing?".
    string rank_4_help()
    {
        string result = "";

        if (input.size() == 1)
        {
            for (keychain_t keychain : rank_4_keychains)
            {
                for (unsigned int i = 0; i < keychain.in.size(); i++)
                {
                    if (input[0] == keychain.in[i])
                    {
                        prep_sent(keychain);
                        result = rand_out(keychain.out);
                        return result;
                    }
                }
            }
            vector<string> options = {input[0] + "...?", "Um, what do you mean by \"" + input[0] + "\"?", input[0] + "? Like, " + input[0] + " what?", input[0] + ". Right.", input[0] + "..."};
            result = rand_out(options);
        }
        return result;
    }

    // Function: find_verb()
    // Sets verb, verb_pres, verb_ed,
    // and verb_ing.
    void find_verb()
    {
        int current_verb_set;
        for (string input_word : input)
        {
            current_verb_set = -1;
            for (vector<string> verb_set : verb_sets)
            {
                current_verb_set++;
                for (string verb_key : verb_set)
                {
                    if (input_word.find(verb_key) == 0) // to avoid "hat" being mistakenly found in "that"
                    {
                        verb_pres = verb_key; // Make all of these
                        verb_ed = verb_key;   // equal the verb key.
                        verb_ing = verb_key;  // Next, we will edit them.

                        // E.g. "mop" becomes "mopp"
                        if (current_verb_set == 0)
                        {
                            char last = verb_key[verb_key.size() - 1];
                            verb_ed += last;
                            verb_ing += last;
                        }

                        // E.g. "despis" becomes "despise"
                        if (current_verb_set == 2)
                            verb_pres += "e";

                        // E.g. "cr" becomes "cry"
                        if (current_verb_set == 3)
                            verb_pres += "y";

                        // Finally add "ed" and "ing"
                        // E.g. "mopp" becomes "mopped" and "mopping"
                        verb_ed += "ed";
                        verb_ing += "ing";

                        // Store the actual input verb in verb variable
                        verb = input_word;
                        return;
                    }
                }
            }
        }
        return;
    }

    // Function: find_tense()
    // A simple way to determine tense
    // is to search for two things:
    // the before-verb and in-verb keys.
    // E.g. "will have been <verb>-ing"
    // is Future Perfect Progressive.
    // So the before-verb is "will have been"
    // and the in-verb is "ing".
    tense find_tense()
    {
        for (unsigned int i = 0; i < tense_help.tenses.size(); i++)
        {
            // if the before-verb key is found
            if (input_str.find(tense_help.before[i]) != string::npos)
            {
                // if the in-verb key is also found
                if (input_str.find(tense_help.in_verb[i]) != string::npos)
                    // return the corresponding tense
                    return tense_help.tenses[i];
            }
        }
        return NONE;
    }

    // Function: find_be()
    // Depending on tense and subject,
    // chooses the appropriate version
    // of "be" (or "do" or "had").
    string find_be()
    {
        if (input_tense == PAST_PERF || input_tense == PAST_PERFPRO)
            return "had";
        if (subject == "i")
        {
            if (input_tense == PAST_PRO)
                return "was";
            if (input_tense == PRES_PRO)
                return "am";
            if (input_tense == PRES_PERF || input_tense == PRES_PERFPRO)
                return "have";
        }
        if (subject == "you" || subject == "they" || subject == "we")
        {
            if (input_tense == PAST_PRO)
                return "were";
            if (input_tense == PRES_PRO)
                return "are";
            if (input_tense == PRES_PERF || input_tense == PRES_PERFPRO)
                return "have";
        }
        if (subject == "she" || subject == "he" || subject == "it")
        {
            if (input_tense == PAST_PRO)
                return "was";
            if (input_tense == PRES_PRO)
                return "is";
            if (input_tense == PRES_PERF || input_tense == PRES_PERFPRO)
                return "has";
        }
        return "";
    }

    // Function: find_rest()
    // Finds the rest of the input following
    // verb.
    // I need to find rest.
    void find_rest()
    {
        vector<string>::iterator it = find(input.begin(), input.end(), verb);
        int index = distance(input.begin(), it);
        for (unsigned int i = index + 1; i < input.size(); i++)
        {
            rest += " ";
            if (input[i] == "me")
                rest += "you";
            else if (input[i] == "you")
                rest += "me";
            else if (input[i] == "myself")
                rest += "yourself";
            else if (input[i] == "yourself")
                rest += "myself";
            else if (input[i] == "my")
                rest += "your";
            else if (input[i] == "your")
                rest += "my";
            else
                rest += input[i];
        }
    }

    string rank_2_help()
    {
        find_verb();
        if (verb == "")
            return "";

        input_tense = find_tense();
        if (input_tense == NONE)
            return "";

        subject = find_subject_pronoun();
        if (subject == "")
            return "";

        string be = find_be();

        find_rest();

        vector<string> verb_options;

        switch (input_tense)
        {
        case FUT_PERFPRO:
            verb_options = {"Why will " + subject + " have been " + verb + "?", verb + "? Why will " + subject + " have been doing that?", verb_ing + ", right. Cool beans.", "Whatever, " + subject + " may need to reevaluate some priorities."};
            break;
        case FUT_PERF:
            verb_options = {"Why will " + subject + " have " + verb_ed + "?", verb + "? Why will " + subject + " have done that?", "If you want to survive, you'd better run. Oh, sorry, did I say something weird?", subject + ", you say? Meh."};
            break;
        case FUT_PRO:
            verb_options = {"Why will " + subject + " be " + verb_ing + "?", verb + "? Why will " + subject + " be doing that?", "Get a life. Like, " + subject + "shouldn't do that when there are so many better things.", verb_ing + " is so last season, and it's not coming back."};
            break;
        case FUT_SIMP:
            verb_options = {"Why will " + subject + " " + verb_pres + "?", verb_ing + ", huh.", subject + " will? Got a reason?", "Why do you say that?"};
            break;
        case PRES_PERFPRO:
            verb_options = {"Why " + be + " " + subject + " been " + verb_ing + "?", verb + "? Why?", "Sure, " + verb_ing + ", I get it. Keep going.", "What do you mean?"};
            break;
        case PRES_PERF:
            verb_options = {"Why " + be + " " + subject + " " + verb + "?", verb + "? Why will " + subject + "-- nevermind, whatever.", subject + " should get a better hobby.", "...Sounds like a totally wicked time."};
            break;
        case PRES_PRO:
            verb_options = {"Why " + be + " " + subject + " " + verb + "?", verb + "? Why " + be + " " + subject + " doing that?", subject + " " + be + " " + verb + rest + "? Elaborate.", "What do you mean?"};
            break;
        case PRES_SIMP:
            verb_options = {"Why " + be + " " + subject + " " + verb + "?", verb + "? Why " + be + " " + subject + " doing that?", "Uh, what? Sounds like a pain, to be honest.", "What do you mean?"};
            break;
        case PAST_PERFPRO:
            verb_options = {"Why " + be + " " + subject + " been " + verb + "?", "These winds are crazy. The winds of life, I mean. I think we should stop talking about this.", "Might not be a good idea.", subject + " what? What an interesting being."};
            break;
        case PAST_PERF:
            verb_options = {"Why " + be + " " + subject + " " + verb_ed + rest + "?", verb + "? Why, though?", "Keep going.", "Right."};
            break;
        case PAST_PRO:
            verb_options = {subject + " " + verb + rest + "?", verb + "? Why would " + subject + " do that?", "Dude, whatevs.", "Wanna switch topics?"};
            break;
        case PAST_SIMP:
            verb_options = {"Why did " + subject + " " + verb_pres + rest + "?", verb + "? Why would " + subject + " do that?"};
            break;
        default:
            break;
        }
        return rand_out(verb_options);
    }

    // Function: refresh()
    // Resets variables that need to be reset
    void refresh()
    {
        input_str.clear();
        output.clear();
        subject.clear();
        object.clear();
        verb.clear();
        verb_pres.clear();
        verb_ed.clear();
        verb_ing.clear();
        rest.clear();
        input_tense = NONE;
        input.clear();
    }

public:
    Chatbot(const string x)
    {
        bot_name = x;
        user_name = "your name";
        wonderful_phrase_sent = false;
        input_tense = NONE;
        rest = "";
    }

    // Function: get_name()
    // Self-explanatory
    string get_name()
    {
        return bot_name;
    }

    // Function: tell()
    // Takes and stores user input.
    // Edits user input before
    // storing in input vector.
    // Also finds user name if mentioned.
    void tell(string user_input)
    {
        past_inputs.push_back(input_str);
        refresh();
        input_str = user_input;
        edit_input(input, input_str);
        find_name();
    }

    // Function: get_reply()
    // Gets an output depending on input.
    // First checks if input is empty or
    // is an abnormal repeat. If not,
    // goes through different ranks of
    // keywords. If an output is still
    // unfound, pick an misc output.
    string get_reply()
    {
        if (output == "")
            output = empty_help();

        if (output == "")
            output = repeat_help();

        // Rank 0: "Hakuna Matata"
        if (output == "")
            output = rank_0_help();

        // Rank 1: High-ranking keywords
        if (output == "")
            output = rank_1_help();

        // Rank 2: Verb keywords
        if (output == "")
            output = rank_2_help();

        // Rank 3: Various other keywords
        if (output == "")
            output = rank_3_help();

        // Rank 4: Single-word keywords
        if (output == "")
            output = rank_4_help();

        // If no output has been chosen, use a miscellaneous one
        if (output == "")
            output = rand_out(misc_out);

        output[0] = toupper(output[0]);
        return output;
    }
};