using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using UnityEngine;

public class RobotMovementSimple : MonoBehaviour
{
    public float rotationSpeed = 145f;  // 自转速度
    public float moveSpeed = 1.5f;       // 移动速度
    public float moveDistance = 0.3f;    // 移动范围

    private Vector3 startPos;

    void Start() => startPos = transform.position;

    void Update()
    {
        // 自转
        transform.Rotate(Vector3.up, rotationSpeed * Time.deltaTime);

        // 往返平移
        float offset = Mathf.PingPong(Time.time * moveSpeed, moveDistance);
        transform.position = startPos + Vector3.right * offset;
    }
}