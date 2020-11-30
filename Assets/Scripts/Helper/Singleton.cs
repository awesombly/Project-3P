using UnityEngine;

public class Singleton<Type> : MonoBehaviour where Type : MonoBehaviour
{
    private static Type instance;

    public static Type Instance
    {
        get
        {
            if ( ReferenceEquals( instance, null ) )
            {
                instance = ( Type )FindObjectOfType( typeof( Type ) );

                if ( ReferenceEquals( instance, null ) )
                {
                    GameObject singleton_object = new GameObject();
                    instance = singleton_object.AddComponent<Type>();
                    singleton_object.name = typeof( Type ).ToString() + " (Singleton)";

                    DontDestroyOnLoad( singleton_object );
                }
            }
            return instance;
        }
    }
}
