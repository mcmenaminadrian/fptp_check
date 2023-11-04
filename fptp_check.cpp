#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <chrono>
#include <random>

using namespace std;

class Voter {
    private:
        unsigned int party;
        bool valid;
    public:
        Voter(unsigned int partyIn);
        unsigned int getParty();
        bool isValid();
        void invalidate();
};

Voter::Voter(unsigned int partyIn)
{
    party = partyIn;
    valid = true;
}

unsigned int Voter::getParty()
{
    return party;
}

bool Voter::isValid()
{
    return valid;
}

void Voter::invalidate()
{
    valid = false;
}

class Party {
    private:
        unsigned int votes;
        string name;
    public:
        Party(string nameIn, unsigned int votesIn);
        string getName();
        unsigned int getVotes();
};

Party::Party(string nameIn, unsigned int votesIn)
{
    name = nameIn;
    votes = votesIn;
}

string Party::getName()
{
    return name;
}

unsigned int Party::getVotes()
{
    return votes;
}

class Results {
    private:
        unsigned int constituencies;
        unsigned int voters;
        unsigned int averageTurnout;
        vector<Party> parties;
    public:
        Results(unsigned int votesIn, unsigned int divisions);
        unsigned int getAverageTurnout();
        void pushParty(Party partyIn);
        unsigned int getPartyCount();
        string getPartyName(unsigned int i);
        unsigned int getPartyVotes(unsigned int i);
        unsigned int getConstituencies();
};

Results::Results(unsigned int votesIn, unsigned int divisions)
{
    voters = votesIn;
    constituencies = divisions;
    averageTurnout = voters / constituencies;
}

unsigned int Results::getAverageTurnout()
{
    return averageTurnout;
}

unsigned int Results::getConstituencies()
{
    return constituencies;
}

void Results::pushParty(Party partyIn)
{
    parties.push_back(partyIn);
}

unsigned int Results::getPartyCount()
{
    return parties.size();
}

string Results::getPartyName(unsigned int i)
{
    if (i > parties.size()) {
        cout << "Party index out of bounds. Halting." << endl;
        exit(0);        
    }
    return parties.at(i).getName();
}

unsigned int Results::getPartyVotes(unsigned int i)
{
    if (i > parties.size()) {
        cout << "Party index out of bounds. Halting." << endl;
        exit(0);        
    }
    return parties.at(i).getVotes();
}
class Electorate {
    private:
        list<Voter> citizens;
        list<Voter>::iterator roamer;
    public:
        void addElector(Voter v);
        unsigned int getSizeLeft();
        int voterAt(unsigned int index);
        int pickNextVoter(int movement);
};

void Electorate::addElector(Voter v)
{
    citizens.push_back(v);
}

unsigned int Electorate::getSizeLeft()
{
    return citizens.size();
}

int Electorate::voterAt(unsigned int index)
{
    int retVal = -1;
    roamer = citizens.begin();
    advance(roamer, index);
    if (roamer->isValid()) {
        retVal = roamer->getParty();
        roamer->invalidate();
    }
    return retVal;
}

int Electorate::pickNextVoter(int difference)
{
    int retVal = -1;
    bool gotValid = false;
    if (difference > 0) {
        roamer++;
        while (!gotValid) {
            if (roamer == citizens.end()) {
                roamer = citizens.begin();
            }
            if (roamer->isValid()) {
                gotValid = true;
                retVal = roamer->getParty();
                roamer->invalidate();
            } else {
                roamer++;
            }
        }
    } else {
        if (roamer == citizens.begin()) {
            roamer = citizens.end();
        }
        roamer--;
        while (!gotValid) {
            if (roamer->isValid()) {
                gotValid = true;
                retVal = roamer->getParty();
                roamer->invalidate();
            } else {
                if (roamer == citizens.begin()) {
                    roamer = citizens.end();
                }
                roamer--;
            }
        }
    }
    return retVal;
}

int main(int argc, char* argv[])
{
    // initialise random numbers
    default_random_engine eng{static_cast<long unsigned int>(time(0))};
    // Create Scottish results
    cout << "Generating party results" << endl;
    Results scotland(2759061, 59);
    Party snp("SNP", 1242380);
    scotland.pushParty(snp);
    Party conservative("Conservative", 692939);
    scotland.pushParty(conservative);
    Party labour("Labour", 511838);
    scotland.pushParty(labour);
    Party libdems("Liberal Democrats", 263417);
    scotland.pushParty(libdems);
    Party greens("Scottish Greens", 28122);
    scotland.pushParty(greens);
    Party brexit("Brexit", 13243);
    scotland.pushParty(brexit);
    Party ukip("UKIP", 3303);
    scotland.pushParty(ukip);
    Party others("others", 3819);
    scotland.pushParty(others);

    // generate an electorate
    cout << "Generating electorate" << endl;
    Electorate electorate;
    const unsigned int partyCount = scotland.getPartyCount();
    for (int i = 0; i < partyCount; i++)
    {
        cout << "Generating " << scotland.getPartyName(i) << " voters." << endl;
        unsigned int totalPartyVotes = scotland.getPartyVotes(i);
        for (int j = 0; j < totalPartyVotes; j++)
        {
            //add to the list
            Voter newVoter = Voter(i);
            electorate.addElector(newVoter);
        }
        cout << "Generation of " << scotland.getPartyName(i) << " voters complete." << endl;
    }
    cout << "Now creating results" << endl;
    const unsigned int averageTurnout = scotland.getAverageTurnout();
    const int max_list = electorate.getSizeLeft();
    vector<int> victories(partyCount, 0);
    uniform_int_distribution<> distrib(0, max_list - 1);
    uniform_int_distribution<> direction(-99, 100);
    for (int i = 0; i < scotland.getConstituencies(); i++)
    {
        cout << "Generating result for constituency " << i + 1 << ":" << flush;
        vector<int> votes(partyCount, 0);
        //initial step is for whole domain - until we find a valid voter
        bool voterValid = false;
        int selected;
        int partyVote;
        while (!voterValid) {
            selected = distrib(eng);
            partyVote = electorate.voterAt(selected);
            if (partyVote >= 0) {
                voterValid = true;
            }
        }
        (votes.at(partyVote))++;
        for (int j = 1; j < averageTurnout; j++)
        {
            int selected = direction(eng);
            unsigned int partyVote = electorate.pickNextVoter(selected);
            (votes.at(partyVote))++;
        }
        for (int j = 0; j < partyCount; j++)
        {
            cout << scotland.getPartyName(j) << ":" << votes.at(j) << "  ";
        }
        cout << endl;
        //calculate winner
        int winningParty = partyCount - 1;
        for (int m = 0; m < partyCount - 1; m++)
        {
            bool winnerFound = true;
            for (int n = 0; n < partyCount; n++)
            {
                if (votes.at(m) < votes.at(n)) {
                    winnerFound = false;
                    break;
                }
            }
            if (winnerFound) {
                winningParty = m;
                break;
            }
        }
        (victories.at(winningParty))++;
        cout << "Running score: ";
        for (int m = 0; m < partyCount; m++)
        {
            cout << scotland.getPartyName(m) << ": " << victories.at(m) << "   ";
        }
        cout << endl;
    }
}