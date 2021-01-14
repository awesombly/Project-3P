using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Entrance : MonoBehaviour
{
    public SceneType enterScene;

    private void OnTriggerEnter( Collider _collider )
    {
        Player player = _collider.GetComponent<Player>();
        if ( player == null || !player.isLocal )
        {
            return;
        }

        // 플레이어 Visible False
        // 해당 플레이어에게 던전 입장 UI Visible True

        // 1. 던전 입장 할 경우
        SceneBase.ChangeScene( enterScene.ToString() );

        // 2. 던전 입장 안할 경우
        // UI Visible False
        // Player Visible True
        // 또는 Village Spawn Position으로 가도록 함
    }
}
