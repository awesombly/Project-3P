using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Garbage : Actor, IInteractable
{
    [System.Serializable]
    public struct DestroyInfo
    {
        public GameObject EffectPrefab;
    }
    public DestroyInfo destroyInfo;

    [SerializeField]
    private string focusMessage;
    public string FocusMessage
    {
        get { return focusMessage; }
        set { focusMessage = value; }
    }

    public void Interaction( Player _player )
    {
        if ( destroyInfo.EffectPrefab != null )
        {
            Instantiate( destroyInfo.EffectPrefab, transform.position, transform.rotation );
        }

        Destroy( gameObject );
    }
}
