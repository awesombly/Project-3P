using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public interface IInteractable
{
    string FocusMessage { get; set; }

    void Interaction( Player _player );
}
