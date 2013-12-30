


// write section
void %%Classname%%::set%%propname%%(%%proptype%% val)
{
    // if this call comes from undo framework, we must hijack the property call
    if (Undoer::instance()->noundo() == false)
    {
        set%%Propname%%Imp(val);
        return;
    }

    if (val == _%%propname%%)
        return;

    if (!validate%%Propname%%(val))
        return;

    QUndoCommand *cmd = new PropertyChangeCmd(this, "%%propname%%", QVariant(_%%propname%%), QVariant(val));
    cmd->setText("Set %%Propname%%");
    Undoer::instance()->push(cmd);
}

void %%Classname%%::set%%Propname%%Imp(%%proptype%% val)
{
    _%%propname%% = val;
    //notify section
    emit %%propname%%Changed();
}

// validate section
bool %%Classname%%::validate%%Propname%%(int val) const
{
    return %%validate%%;
}

