public:
    Q_PROPERTY(%%proptype%% %%propname%% READ %%propname%% WRITE set%%Propname%% NOTIFY %%propname%%Changed)
    bool validate%%Propname%%(%%proptype%% val) const;
    %%proptype%% %%propname%%() const;
    void set%%Propname%%(%%proptype%% val);
signals:
    void %%propname%%Changed();
private:
    void set%%Propname%%Imp(%%proptype%% val);
    int _%%propname%%;
