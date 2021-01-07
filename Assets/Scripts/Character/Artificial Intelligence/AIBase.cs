using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public abstract class AIBase : Character
{
    public enum AIState : byte
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

    internal bool IsSpawn = false;

    protected NavMeshAgent nav { get; private set; }
    protected Vector3 target;
    protected AIState state;
    
    private Coroutine currentCoroutine = null;

    public void SyncState( Vector3 _target, Vector3 _curPosition )
    {
        target = _target;
        transform.position = _curPosition;
    }

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
        state = _state;

        if ( !ReferenceEquals( currentCoroutine, null ) )
        {
            StopCoroutine( currentCoroutine );
        }
        animator.SetInteger( AnimatorParameters.AIState, ( int )_state );

        if ( Network.Instance.isConnected )
        {
            Protocol.Both.SyncNpcState protocol;
            protocol.NpcInfo.NpcId = gameObject.name;
            protocol.NpcInfo.Target = target;
            protocol.NpcInfo.CurPosition = transform.position;

            Network.Instance.Send( protocol );
        }

        currentCoroutine = StartCoroutine( _state.ToString() );
    }

    protected abstract IEnumerator Idle();

    protected virtual void OnExit()
    {
        StopAllCoroutines();
    }
}