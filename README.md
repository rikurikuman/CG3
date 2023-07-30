# RKEngine
これは、WindowsAPIやDirectX12を用いることで様々な計算やレンダリングを行うゲームエンジン(?)です。  
使う人が直感的で書きやすいこと、汎用性が高く楽に使える機能であることを意識しています。  
まだまだ至らぬ点だらけなため、日々実用しながら改善中です。

### 機能
- Rendererクラスにより全てが一括で管理された2D/3Dの描画
  - DrawBox, DrawCircle, DrawStringなどのシンプルなプリミティブ描画が1行で行える関数
  - 3Dモデルの読み込み/描画
  - 各種3D描画を行えるクラス、その基底となる汎用的な機能群
  - マルチパスレンダリング
- 各種Colliderクラスによる当たり判定
  - Sphere, Ray, Polygon
- XAudio2によるサウンドの再生
  - 再生, 停止, 音量/ピッチの調整, ループ位置指定再生, 現在の再生位置取得
- DirectInput, XInputによる入力
  - キーボード, マウス, ゲームパッド(XInput)
- 独自のバッファアロケータによる一括GPUリソースメモリ管理
  - 頂点, インデックス, 定数バッファ等の確保と解放が高速に行える  
    (弾幕の1発1発のコンストラクタで確保していても問題ない程度)
  - 事前に多めに確保してstd::vectorのようなコンテナにプールしておく機能との相性良好
### 使用している外部のライブラリ
- [DirectXTex](https://github.com/microsoft/DirectXTex)  
- [Dear ImGui](https://github.com/ocornut/imgui)  
- [Open Asset Importer Library(assimp)](https://github.com/assimp/assimp)  
  
これらのライブラリには大変助かっています。
