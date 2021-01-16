using System.Collections;
using System.Collections.Generic;
using UnityEngine;


[CreateAssetMenu( fileName = "Emotion", menuName = "Scriptable Object/Emotion" )]
public class Emotion : Item
{
    public string animatorParameter;

    private int parameterHash;

    public override void OnUseItem( Player _player )
    {
        _player.animator.SetTrigger( parameterHash );

        if ( _player.isLocal )
        {
            SendSyncProtocol( _player );
        }
    }

    private void OnEnable()
    {
        itemType = EItemType.Emotion;
        parameterHash = Animator.StringToHash( animatorParameter );
    }
}