using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraController : MonoBehaviour
{
    [SerializeField] private Transform m_Target;

    void Update()
    {
        transform.LookAt(m_Target);        
    }
}
