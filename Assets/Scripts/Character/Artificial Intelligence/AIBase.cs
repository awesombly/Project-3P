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

    public int state { get; private set; }
    public Vector3 target { get; protected set; }

    private Coroutine currentCoroutine = null;

    protected NavMeshAgent nav { get; private set; }
    private float moveSpeed = 5.0f;
    private float angularSpeed = 1000.0f;
    private float acceleration = 20.0f;

    public void Sync( Vector3 _target, Vector3 _position, int _state )
    {
        target = _target;
        transform.position = _position;
        ChangeState( ( AIState )_state );
    }

    protected override void Awake()
    {
        base.Awake();

        isLocal = false;
        Network.Instance.OnLateConnect += OnLateConnect;

        nav = GetComponent<NavMeshAgent>();
        nav.speed = moveSpeed;
        nav.angularSpeed = angularSpeed;
        nav.acceleration = acceleration;
    }

    protected virtual void Start()
    {
        ChangeState( AIState.Idle );
    }

    protected override void FixedUpdate()
    {
        
    }

    protected virtual void OnLateConnect()
    {
        Protocol.ToServer.RequestNpcInfo requestNpcInfo;
        requestNpcInfo.NpcId = gameObject.name;

        Network.Instance.Send( requestNpcInfo );
    }

    protected void ChangeState( AIState _state )
    {
        if ( !ReferenceEquals( currentCoroutine, null ) )
        {
            StopCoroutine( currentCoroutine );
        }

        state = ( int )_state;
        animator.SetInteger( AnimatorParameters.AIState, ( int )_state );

        if ( isLocal && Network.Instance.isConnected )
        {
            Protocol.ToServer.ResponseCriterionNpcInfo protocol;
            protocol.NpcInfo.IsLocal = isLocal;
            protocol.NpcInfo.Serial = serial;
            protocol.NpcInfo.State = state;
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