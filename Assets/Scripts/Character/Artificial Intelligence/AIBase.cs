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
        Interaction,
    };

    public int state { get; private set; }
    
    public Vector3 target { get; protected set; }

    protected NavMeshAgent nav { get; private set; }
    private float moveSpeed = 5.0f;
    private float angularSpeed = 1000.0f;
    private float acceleration = 20.0f;

    protected float disSqrInteraction { get; private set; }
    protected bool isInteraction = false;
    protected Player focusedPlayer;
    private readonly float disInteraction = 2.0f;
    private readonly WaitForSeconds DistanceCheckCached = new WaitForSeconds( 0.1f );


    private Coroutine currentCoroutine = null;

    public void Sync( Vector3 _target, Vector3 _position, int _state )
    {
        target = _target;
        transform.position = _position;
        ChangeState( ( AIState )_state );
    }

    public void SyncInteration( Player _player )
    {
        focusedPlayer = _player;
        ChangeState( AIState.Interaction );
        isInteraction = true;
    }

    private IEnumerator CheckPlayerDistance()
    {
        while ( true )
        {
            yield return DistanceCheckCached;

            if ( isInteraction )
            {
                continue;
            }

            foreach ( Player player in ObjectManager.Instance.Players )
            {
                if ( player.isLocal && ( transform.position - player.transform.position ).sqrMagnitude <= disSqrInteraction )
                {
                    Protocol.Both.SyncNpcInteraction protocol;
                    protocol.NpcSerial = serial;
                    protocol.PlayerSerial = player.serial;
                    Network.Instance.Send( protocol );

                    SyncInteration( player );
                    break;
                }
            }
        }
    }

    protected override void Awake()
    {
        base.Awake();

        disSqrInteraction = Mathf.Pow( disInteraction, 2 );

        isLocal = false;
        Network.Instance.OnLateConnect += OnLateConnect;

        nav = GetComponent<NavMeshAgent>();
        nav.speed = moveSpeed;
        nav.angularSpeed = angularSpeed;
        nav.acceleration = acceleration;

        StartCoroutine( CheckPlayerDistance() );
    }

    protected virtual void Start()
    {
        ChangeState( AIState.Idle );
    }

    protected override void FixedUpdate()
    {
        
    }

    protected virtual void OnDestroy()
    {
        Network.Instance.OnLateConnect -= OnLateConnect;
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
            Protocol.ToServer.ResponseHostNpcInfo protocol;
            protocol.NpcInfo.IsLocal = isLocal;
            protocol.NpcInfo.Actor.Serial = serial;
            protocol.NpcInfo.Actor.Position = transform.position;
            protocol.NpcInfo.Actor.Rotation= transform.rotation;
            protocol.NpcInfo.State = state;
            protocol.NpcInfo.NpcId = gameObject.name;
            protocol.NpcInfo.Target = target;

            Network.Instance.Send( protocol );
        }

        currentCoroutine = StartCoroutine( _state.ToString() );
    }

    protected abstract IEnumerator Idle();
    protected abstract IEnumerator Interaction();

    protected virtual void OnExit()
    {
        StopAllCoroutines();
    }
}