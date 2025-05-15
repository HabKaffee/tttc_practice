class Dota2Player {
public:
    const char* nickname;
    int mmr;
    int yearsPlaying;
    int majorsWon;

    virtual void pickHero() = 0;
    virtual void playRanked() = 0;
    virtual void callGG() = 0;
    virtual void signAutographs() = 0;
};

class Midlaner : public Dota2Player {
public:
    bool prefersStormSpirit;
    int averageCSAt10;
    int soloKillsPerGame;

    void pickHero() override {}
    void playRanked() override {}
    void callGG() override {}
    void signAutographs() override {}

    void gankSideLanes() {}
    void spamVoiceLines() {}
    void soloKillEnemyMid() {}
};

