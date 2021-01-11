﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;


[CreateAssetMenu( fileName = "Emotion", menuName = "Scriptable Object/Emotion" )]
public class Emotion : Item
{
    public string animatorParameter;

    public override void OnUseItem( Player _player )
    {
        _player.animator.SetTrigger( animatorParameter );
    }

    private void OnEnable()
    {
        itemType = EItemType.Emotion;
    }
}