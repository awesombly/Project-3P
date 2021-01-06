using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public abstract class AIBase : Character
{
    protected enum AIState : byte
    {
        /* 기본 상태 */
        Idle = 0,

        /* 추가 상태 */
        Attack,
        Patrol,
        Move = 3,
        Dash,
        Dead,
    };

    protected NavMeshAgent nav { get; private set; }
    // protected AIState state { get; private set; }

    private Coroutine currentCoroutine = null;

    protected override void Awake()
    {
        base.Awake();

        isLocal = false;
        nav = GetComponent<NavMeshAgent>();
    }

    protected virtual void Start()
    {
        ChangeState( AIState.Idle );
    }

    protected void ChangeState( AIState _state )
    {
        if ( !ReferenceEquals( currentCoroutine, null ) )
        {
            StopCoroutine( currentCoroutine );
        }
        // state = _state;
        animator.SetInteger( AnimatorParameters.AIState, ( int )_state );
        currentCoroutine = StartCoroutine( _state.ToString() );
    }

    protected abstract IEnumerator Idle();

    protected virtual void OnExit()
    {
        StopAllCoroutines();
    }
}