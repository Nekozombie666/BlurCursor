# RubberBand Cursor (ゴム紐カーソル)

**[English follows Japanese]**

マウスカーソルに「ゴム紐」のような物理演算ベースの軌跡を追加する、Windows用デスクトップアクセサリーです。
マウスの動きに合わせて、滑らかで生き物のようなラインが追いかけてきます。

![BlurCursor](https://github.com/user-attachments/assets/f5c5006e-5838-422c-b290-7559ae1cc2d0)

## ✨ 特徴 (Features)

* **物理演算**: バネの法則（フックの法則）を応用した、独特な浮遊感のある動き。
* **軽量・高速**: 描画範囲を最小限にする最適化（Dirty Rectangle方式）により、低スペックPCや高解像度環境でもヌルヌル動きます。
* **マルチモニター対応**: 複数の画面を行き来しても途切れません。
* **フルカスタマイズ**:
    * 紐の長さ
    * バネの硬さ（追従速度）
    * 線の太さ
    * カラー変更（パレット対応）
* **タスクトレイ常駐**: 設定画面を閉じてもバックグラウンドで動作し、作業の邪魔になりません。
* **設定保存**: 次回起動時に前回のお気に入り設定を自動で読み込みます。

## 📦 ダウンロードと使い方 (Usage)

1.  右側の **[Releases]** ページから最新の `.zip` ファイルをダウンロードしてください。
2.  解凍して `RubberBand.exe` を実行します。
    * *※「WindowsによってPCが保護されました」と出る場合: 「詳細情報」→「実行」をクリックしてください。*
3.  タスクトレイ（画面右下の時計の横）にあるアイコンを**左クリック**すると設定画面が開きます。
4.  アイコンを**右クリック**すると終了できます。

## 🛠️ 開発環境 (Tech Stack)

* **Language**: C++
* **Library**: Win32 API (GDI)
* **Compiler**: MinGW (g++)

## 🤖 AI Development

このアプリケーションのコードは、**AI (Google Gemini)** との対話・ペアプログラミングによって作成されました。
C++やWin32 APIの複雑な実装（ウィンドウ処理、描画最適化、タスクトレイ機能など）をAIがサポートしています。

This application's code was generated through dialogue and pair programming with **AI (Google Gemini)**.

## 📝 ビルド方法 (Build from source)

開発環境 (MinGW/g++) がある場合、以下のコマンドでビルド可能です。

```bash
g++ Untitled-1.cpp -o RubberBand.exe -lgdi32 -lcomctl32 -lshell32 -lcomdlg32 -mwindows -static
