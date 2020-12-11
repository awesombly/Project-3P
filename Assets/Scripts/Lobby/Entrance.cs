using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Entrance : MonoBehaviour
{
    private void OnCollisionEnter( Collision _collision )
    {
        if ( _collision.collider.CompareTag( "Player" ) )
        {
            // 플레이어 Visible False
            // 해당 플레이어에게 던전 입장 UI Visible True

            // 1. 던전 입장 할 경우
            // 씬 변경
            SceneManager.LoadScene( "Dungeon" );
            // 서버에 던전 안 특정 위치에 스폰되도록 패킷 전송

            // 2. 던전 입장 안할 경우
            // UI Visible False
            // Player Visible True
            // 또는 Village Spawn Position으로 가도록 함
        }
    }
}
