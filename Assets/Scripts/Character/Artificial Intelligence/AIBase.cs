using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class AIBase : MonoBehaviour
{
    protected enum AIState : byte
    {
        /* 기본 상태 */
        Idle = 0,

        /* 추가 상태 */
        Attack,
        Move,
        Dead,
    };

    protected AIState state { get; set; } = AIState.Idle;

    protected virtual void Start()
    {
        StartCoroutine( ChangeState( state ) );
    }

    protected IEnumerator ChangeState( AIState _state )
    {
        StopCoroutine( state.ToString() );
        state = _state;
        yield return StartCoroutine( _state.ToString() );
    }

    protected abstract IEnumerator Idle();

    protected virtual void OnExit()
    {
        StopAllCoroutines();
    }
}
