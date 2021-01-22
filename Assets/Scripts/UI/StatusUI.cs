using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class StatusUI : MonoBehaviour
{
    public Text focusText;
    public Text interactionText;

    private void Awake()
    {
        focusText.text = string.Empty;
        interactionText.text = string.Empty;

        Player.OnChangeFocusTarget += OnChangeFocusTarget;
    }

    private void OnDestroy()
    {
        Player.OnChangeFocusTarget -= OnChangeFocusTarget;
    }

    private void OnChangeFocusTarget( Actor _target )
    {
        if ( _target == null )
        {
            focusText.text = string.Empty;
            interactionText.text = string.Empty;
            return;
        }
        focusText.text = _target.name;

        IInteractable interactor = _target as IInteractable;
        if ( ReferenceEquals( interactor, null) )
        {
            interactionText.text = string.Empty;
            return;
        }
        interactionText.text = interactor.FocusMessage;
    }
}
