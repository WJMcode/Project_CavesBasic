# AI를 활용한 언리얼 엔진 C++ 문제 해결 사례 : 몬스터 사망 연출 구현

## 📌 프로젝트 개요

**프로젝트명:** `CavesBasic` (개인 프로젝트)

언리얼 엔진 C++을 사용하여 몬스터의 **자연스러운 사망 연출**을 구현한 사례입니다.  
AI의 도움을 받아 **Dynamic Material Instance**와 **Timeline 컴포넌트**를 연동하여,
몬스터가 사망 시 **서서히 투명해지며 소멸하는 효과**를 완성했습니다.

---

## 🎯 문제 상황 : 막막했던 몬스터 소멸 효과 구현

기존에는 몬스터가 사망 시 즉시 사라지는 방식이었지만,  
더 자연스러운 연출을 위해 **서서히 투명해지는 시각 효과**를 C++ 코드로 직접 구현하고자 했습니다.

이를 위해 다음과 같은 기능이 필요했습니다.

* 동적 머티리얼 인스턴스(`MID`) 생성 및 제어
* `Opacity` 파라미터 값을 시간에 따라 실시간 변경
* 타임라인(`FTimeline`)을 통한 값 보간 및 이벤트 처리

그러나 공식 문서만으로는 C++에서 이 두 기능을 연동하는 과정이 명확하지 않았습니다.

---

## 💬 AI의 도움 : “개인 교사”를 만나다

AI에게 다음과 같은 질문을 던졌습니다.

> “언리얼 엔진 C++에서 타임라인을 사용해서, Dynamic Material Instance의 스칼라 파라미터 값을 조절하는 방법 알려줘.  
> 몬스터가 죽을 때 서서히 투명해지는 효과를 만들고 싶어.”

AI는 단순한 코드 예시뿐 아니라,

* 타임라인과 커브 플롯의 원리
* MID 생성과 파라미터 제어 방식
* 델리게이트 바인딩의 동작 구조

를 단계적으로 설명해주었습니다.
이 과정을 통해 단순 복사가 아닌 **원리 기반의 학습과 적용**이 가능했습니다.

---

## 💻 해결 과정 : AI의 지식을 내 것으로 만들다

AI가 제시한 개념과 예시 코드를 참고하여,
프로젝트의 `DefaultMonster` 클래스 구조에 맞게 재구성했습니다.

### 🧩 핵심 개념 요약

1. **타임라인 컴포넌트 활용**

   * `FTimeline` 타입 멤버 선언
   * `UCurveFloat` 에셋과 연결하여 시간 기반 값 변화를 정의

2. **델리게이트 바인딩**

   * 타임라인 업데이트 시 호출될 콜백 함수를 생성
   * `FOnTimelineFloat` 델리게이트에 바인딩하여 실시간 값 처리

3. **동적 머티리얼 제어**

   * `DynamicMaterialInstance->SetScalarParameterValue` 함수로
     타임라인 커브 값을 이용해 `Opacity` 파라미터 조절

---

## 🧱 실제 프로젝트에 적용한 최종 코드

```cpp
// DefaultMonster.cpp

// BeginPlay에서 MaterialInstanceDynamic 초기화
MaterialInstanceDynamics = SkeletalMesh->CreateAndSetMaterialInstanceDynamic(1);

// 타임라인 커브에 따라 호출되는 함수
// OnDisappearMesh : Opacity를 줄여 몬스터가 점점 사라지게 함
float NewTransparency = FMath::Max(CurrentTransparency - InDissolve * SpeedMultiplier, 0.0f);
MaterialInstanceDynamics->SetScalarParameterValue(FName("Opacity"), NewTransparency);

// 타임라인 재생이 끝나면 Actor를 완전히 제거
void ADefaultMonster::OnDisappearMeshEnd()
{
    Destroy();
}
```

---

## ✨ 최종 결과

AI와의 협업을 통해,
몬스터가 **점점 투명해지며 자연스럽게 사라지는 연출**을 완성했습니다.
이는 플레이 경험의 완성도를 높이는 데 기여했습니다.

---

## 🔗 관련 링크

* **GitHub Repository :** [Project_CavesBasic](#)
* **전체 소스 코드 :** `DefaultMonster.cpp`

---

> “AI는 단순한 도구가 아니라,
> 학습과 문제 해결 과정을 함께하는 **개인 교사**였습니다.”
