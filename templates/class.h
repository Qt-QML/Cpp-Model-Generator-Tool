#ifndef %%CLASSNAME%%_H
#define %%CLASSNAME%%_H

#include "object.h"

class %%Classname%% : public Object
{
    Q_OBJECT

public:
    Q_INVOKABLE %%Classname%%(QObject *parent = 0);
    ~%%Classname%%();

    // here are the properties
    %%properties%%
};

#endif // %%CLASSNAME%%_H



