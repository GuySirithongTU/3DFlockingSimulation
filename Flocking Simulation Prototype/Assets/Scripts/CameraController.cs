using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraController : MonoBehaviour
{
    [SerializeField] private float m_YawPitchSpeed;
    [SerializeField] private float m_MoveSpeed;

    private float m_Yaw;
    private float m_Pitch;

    private void Start()
    {
        m_Yaw = transform.localEulerAngles.y;
        m_Pitch = transform.localEulerAngles.x;
    }

    private void Update()
    {
        // lock cursor
        if(Input.GetMouseButtonDown(1)) {
            Cursor.lockState = CursorLockMode.Locked;
        } else if(Input.GetMouseButtonUp(1)) {
            Cursor.lockState = CursorLockMode.None;
        }

        // pitch and yaw
        if(Input.GetMouseButton(1)) {
            m_Yaw += m_YawPitchSpeed * Input.GetAxis("Mouse X");
            m_Pitch -= m_YawPitchSpeed * Input.GetAxis("Mouse Y");

            m_Pitch = Mathf.Clamp(m_Pitch, -89.0f, 89.0f);

            transform.eulerAngles = new Vector3(m_Pitch, m_Yaw, 0.0f);
        }

        // move
        Vector3 translate = Vector3.zero;
        translate += Vector3.forward * Input.GetAxis("Vertical");
        translate += Vector3.right * Input.GetAxis("Horizontal");
        translate += Vector3.up * Input.GetAxis("Jump");
        transform.Translate(translate * m_MoveSpeed);
    }
}
