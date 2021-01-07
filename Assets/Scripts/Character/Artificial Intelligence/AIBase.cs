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
    
    protected Vector3 target;
    private Coroutine currentCoroutine = null;

    protected NavMeshAgent nav { get; private set; }
    private float moveSpeed = 1.5f;
    private float angularSpeed = 1000.0f;
    private float acceleration = 20.0f;

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
        nav.speed = moveSpeed;
        nav.angularSpeed = angularSpeed;
        nav.acceleration = acceleration;
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