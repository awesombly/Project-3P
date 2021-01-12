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

    protected bool isInteraction = false;

    private Coroutine currentCoroutine = null;
    private Player focusedPlayer = null;

    public void Sync( Vector3 _target, Vector3 _position, int _state )
    {
        transform.position = _position;
        target = _target;
        ChangeState( ( AIState )_state );
    }

    public void SyncTarget( Vector3 _target )
    {
        target = _target;
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

    protected virtual void OnTriggerEnter( Collider _other )
    {
        if ( !isLocal )
        {
            return;
        }

        if ( _other.CompareTag( "Player" ) )
        {
            if ( isInteraction )
            {
                return;
            }

            if ( focusedPlayer == null )
            {
                focusedPlayer = _other.GetComponent<Actor>() as Player;
                StopAllCoroutines();
                target = _other.transform.position;
                ChangeState( AIState.Interaction );
            }
        }
    }

    protected virtual void OnTriggerStay( Collider _other )
    {
        if ( !isLocal )
        {
            return;
        }

        if ( isInteraction && _other.CompareTag( "Player" ) )
        {
            float Offset = ( target - _other.transform.position ).sqrMagnitude;
            if ( Offset >= 0.05f )
            {
                target = _other.transform.position;

                Protocol.Both.SyncNpcTarget protocol;
                protocol.Serial = serial;
                protocol.Target = target;

                Network.Instance.Send( protocol );
            }
        }
    }

    protected virtual void OnTriggerExit( Collider _other )
    {
        if ( !isLocal )
        {
            return;
        }

        if ( isInteraction && _other.CompareTag( "Player" ) )
        {
            if ( focusedPlayer != null &&
                !focusedPlayer.serial.Equals( _other.GetComponent<Actor>().serial ) )
            {
                return;
            }

            focusedPlayer = null;
            StopAllCoroutines();
            ChangeState( AIState.Idle );
        }
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

        isInteraction = false;
        nav.isStopped = false;

        nav.avoidancePriority = 50;

        state = ( int )_state;
        animator.SetInteger( AnimatorParameters.AIState, ( int )_state );

        if ( isLocal && Network.Instance.isConnected )
        {
            Protocol.ToServer.ResponseHostNpcInfo protocol;
            protocol.NpcInfo.Actor.Serial = serial;
            protocol.NpcInfo.Actor.Position = transform.position;
            protocol.NpcInfo.Actor.Rotation = transform.rotation;
            protocol.NpcInfo.IsLocal = isLocal;
            protocol.NpcInfo.State = state;
            protocol.NpcInfo.NpcId = gameObject.name;
            protocol.NpcInfo.Target = target;

            Network.Instance.Send( protocol );
        }

        currentCoroutine = StartCoroutine( _state.ToString() );
    }

    protected abstract IEnumerator Idle();

    protected abstract IEnumerator Interaction();
}