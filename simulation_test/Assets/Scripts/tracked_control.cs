using UnityEngine;

public class RigidbodyRotator : MonoBehaviour
{
    public Vector3 movementAxis = Vector3.forward; // 运动轴，默认X轴
    public float movementSpeed = 0.3f; // 运动速度
    public float movementRange = 1.0f; // 运动范围（单程距离）

    private Rigidbody rb;
    private Vector3 minPosition;
    private Vector3 maxPosition;
    private bool movingToMax = true;
    private float currentDistance = 0f;

    void Start()
    {
        // 获取刚体并设置初始位置
        rb = GetComponent<Rigidbody>();
        rb.useGravity = false;

        // 确保运动轴是单位向量
        movementAxis.Normalize();
        Vector3 startPosition = rb.position;
        // 计算往返运动的起始和结束位置
        Vector3 halfRange = movementAxis * (movementRange / 2f);
        minPosition = startPosition - halfRange;
        maxPosition = startPosition + halfRange;

        // 设置初始位置
    
    }

    void FixedUpdate()
    {
        // 计算这一帧移动的距离
        float moveDistance = movementSpeed * Time.fixedDeltaTime;

        // 更新当前在路径上的位置
        if (movingToMax)
        {
            currentDistance += moveDistance;
            if (currentDistance >= movementRange)
            {
                currentDistance = movementRange;
                movingToMax = false;
            }
        }
        else
        {
            currentDistance -= moveDistance;
            if (currentDistance <= 0f)
            {
                currentDistance = 0f;
                movingToMax = true;
            }
        }

        // 计算新位置
        // 使用插值计算位置：从minPosition到maxPosition
        float t = currentDistance / movementRange;
        Vector3 newPosition = Vector3.Lerp(minPosition, maxPosition, t);

        // 使用刚体移动
        rb.MovePosition(newPosition);
    }
}



// using UnityEngine;

// public class RigidbodyRotator : MonoBehaviour
// {
//     public Vector3 startPosition = new Vector3(0f,0.18f,10f);
//     public Vector3 rotationAxis = Vector3.up;
//     public float rotationSpeed = 5f;

//     private Rigidbody rb;

//     void Start()
//     {
//         // 获取刚体并设置初始位置
//         rb = GetComponent<Rigidbody>();
//         rb.useGravity = false;  

//         rb.position = startPosition;
//     }

//     void FixedUpdate()
//     {
//         // 使用刚体旋转
//         float angle = rotationSpeed * Time.fixedDeltaTime;
//         Quaternion deltaRotation = Quaternion.AngleAxis(angle, rotationAxis);
//         rb.MoveRotation(rb.rotation * deltaRotation);
//     }
// }