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
    
    protected int state { get; private set; }
    protected Vector3 target;
    private Coroutine currentCoroutine = null;

    protected NavMeshAgent nav { get; private set; }
    private float moveSpeed = 1.5f;
    private float angularSpeed = 1000.0f;
    private float acceleration = 20.0f;

    protected override void Awake()
    {
        base.Awake();

        isLocal = false;
        Network.Instance.OnLateConnect += OnLateConnect;
        Network.Instance.OnBindProtocols += OnBindProtocols;

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
        requestNpcInfo.NpcInfo.IsLocal = isLocal;
        requestNpcInfo.NpcInfo.State = state;
        requestNpcInfo.NpcInfo.NpcId = gameObject.name;
        requestNpcInfo.NpcInfo.Target = target;
        requestNpcInfo.NpcInfo.CurPosition = transform.position;
        Network.Instance.Send( requestNpcInfo );
    }

    protected virtual void OnBindProtocols()
    {
        Network.Instance.AddBind( Protocol.FromServer.RequestNpcInfo.PacketType, RequestNpcInfo );
        Network.Instance.AddBind( Protocol.FromServer.ResponseNpcInfo.PacketType, ResponseNpcinfo );
        Network.Instance.AddBind( Protocol.Both.SyncNpcState.PacketType, SyncNpcState );
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
            SendResponseNpcInfo();
        }

        currentCoroutine = StartCoroutine( _state.ToString() );
    }

    protected abstract IEnumerator Idle();

    protected virtual void OnExit()
    {
        StopAllCoroutines();
    }

    private void SendResponseNpcInfo()
    {
        Protocol.ToServer.ResponseNpcInfo protocol;
        protocol.NpcInfo.IsLocal = isLocal;
        protocol.NpcInfo.State = state;
        protocol.NpcInfo.NpcId = gameObject.name;
        protocol.NpcInfo.Target = target;
        protocol.NpcInfo.CurPosition = transform.position;

        Network.Instance.Send( protocol );
    }

    private void SyncNpcState( string _data )
    {
        Protocol.Both.SyncNpcState protocol = JsonUtility.FromJson<Protocol.Both.SyncNpcState>( _data );
        isLocal = protocol.NpcInfo.IsLocal;
        target = protocol.NpcInfo.Target;
        transform.position = protocol.NpcInfo.CurPosition;
        ChangeState( ( AIState )protocol.NpcInfo.State );
    }

    private void RequestNpcInfo( string _data )
    {
        if ( Network.Instance.isConnected )
        {
            SendResponseNpcInfo();
        }
    }

    private void ResponseNpcinfo( string _data )
    {
        Protocol.FromServer.ResponseNpcInfo protocol = JsonUtility.FromJson<Protocol.FromServer.ResponseNpcInfo>( _data );
        isLocal = protocol.IsLocal;
    }
}