using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Character : Actor
{
    internal bool isGrounded = true;
    internal bool isSprinting = false;

    private bool isCrouching = false;
    internal bool IsCrouching
    {
        get { return isCrouching; }
        set 
        {
            if ( isCrouching == value )
            {
                return;
            }

            isCrouching = value;
            OnChangeCrouching?.Invoke( isCrouching );
        }
    }
    public delegate void DelChangeCrouching( bool isCrouch );
    public event DelChangeCrouching OnChangeCrouching;

    private struct SyncMovement
    {
        public const float NeedInterval = 0.1f;
        public Vector3 PrevVelocity;
        public float PrevSqrMagnitude;
    }
    private SyncMovement syncMovement;

    private Animator animator;
    private CapsuleCollider capsule;
    private float originCapsuleHeight;

    private const float AnimationDampTime = 0.2f;
    private float inputVertical;
    private float inputHorizontal;
    private float inputMagnitude;

    protected override void Awake()
    {
        base.Awake();

        OnChangeCrouching += SendSyncCrouch;

        animator = GetComponentInChildren<Animator>();

        capsule = GetComponent<CapsuleCollider>();
        originCapsuleHeight = capsule.height;
    }

    protected override void FixedUpdate()
    {
        base.FixedUpdate();

        UpdateCrouchState();

        UpdateInputParameters();
        UpdateAnimatorParameters();
        
        if ( !isLocal )
        {
            return;
        }

        float velocityInterval = Vector3.Distance( rigidBody.velocity, syncMovement.PrevVelocity );
        if ( velocityInterval > SyncMovement.NeedInterval )
        {
            Protocol.Both.SyncInterpolation protocol;
            protocol.Actor.Serial = serial;
            protocol.Actor.Position = transform.position;
            protocol.Actor.Rotation = transform.rotation;
            protocol.Velocity = rigidBody.velocity;

            Network.Instance.Send( protocol );
        }
        syncMovement.PrevVelocity = rigidBody.velocity;
        syncMovement.PrevSqrMagnitude = rigidBody.velocity.sqrMagnitude;

        /// TODO : 정지중일때 방향전환시 동기화
    }

    private void UpdateCrouchState()
    {
        /// 필요할때만 코루틴으로 처리하는게 좋을듯
        if ( IsCrouching )
        {
            capsule.height = Mathf.MoveTowards( capsule.height, originCapsuleHeight * 0.65f, Time.deltaTime * 5.0f );
        }
        else
        {
            capsule.height = Mathf.MoveTowards( capsule.height, originCapsuleHeight, Time.deltaTime * 5.0f );
        }

        animator.gameObject.transform.localPosition = ( capsule.height * 0.5f * Vector3.down );
    }

    private void UpdateInputParameters()
    {
        float sprintingRate = ( isSprinting ? 1.5f : 1.0f );
        inputVertical = Mathf.Clamp( localVelocity.z, -1.0f, 1.0f ) * sprintingRate;
        inputHorizontal = Mathf.Clamp( localVelocity.x, -1.0f, 1.0f ) * sprintingRate;

        inputMagnitude = localVelocity.normalized.magnitude;
    }

    private void UpdateAnimatorParameters()
    {
        animator.SetBool( AnimatorParameters.IsGrounded, isGrounded );
        animator.SetBool( AnimatorParameters.IsStrafing, true );
        animator.SetBool( AnimatorParameters.IsSprinting, isSprinting );
        animator.SetBool( AnimatorParameters.IsCrouching, isCrouching );

        bool isStop = rigidBody.velocity.sqrMagnitude < float.Epsilon;
        animator.SetFloat( AnimatorParameters.InputHorizontal, isStop ? 0.0f : inputHorizontal, AnimationDampTime, Time.deltaTime );
        animator.SetFloat( AnimatorParameters.InputVertical, isStop ? 0.0f : inputVertical, AnimationDampTime, Time.deltaTime );
        animator.SetFloat( AnimatorParameters.InputMagnitude, isStop ? 0.0f : inputMagnitude, AnimationDampTime, Time.deltaTime );
        animator.SetFloat( AnimatorParameters.VelocityY, rigidBody.velocity.y );
    }

    private void SendSyncCrouch( bool isCrouch )
    {
        if ( !isLocal )
        {
            return;
        }

        Protocol.Both.SyncCrouch protocol;
        protocol.Serial = serial;
        protocol.IsCrouch = isCrouch;
        Network.Instance.Send( protocol );
    }

    private static class AnimatorParameters
    {
        public static int InputHorizontal = Animator.StringToHash( "InputHorizontal" );
        public static int InputVertical = Animator.StringToHash( "InputVertical" );
        public static int InputMagnitude = Animator.StringToHash( "InputMagnitude" );
        public static int VelocityY = Animator.StringToHash( "VelocityY" );
        public static int GroundDistance = Animator.StringToHash( "GroundDistance" );
        public static int IsGrounded = Animator.StringToHash( "IsGrounded" );
        public static int IsStrafing = Animator.StringToHash( "IsStrafing" );
        public static int IsSprinting = Animator.StringToHash( "IsSprinting" );
        public static int IsCrouching = Animator.StringToHash( "IsCrouching" );
    }
}
