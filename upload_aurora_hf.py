#!/usr/bin/env python3
"""
Aurora Trinity-3 - Automated Hugging Face Upload
===============================================
"""

import os
import sys
from pathlib import Path

def upload_to_huggingface():
    """Upload Aurora Trinity-3 to Hugging Face."""
    try:
        from huggingface_hub import HfApi, create_repo
        
        print("🌌 Aurora Trinity-3 - Uploading to Hugging Face")
        print("=" * 60)
        
        # Get user credentials
        print("📝 Please provide your Hugging Face credentials:")
        
        # Get HF token
        hf_token = input("Enter your Hugging Face token: ").strip()
        if not hf_token:
            print("❌ Token is required. Get one from: https://huggingface.co/settings/tokens")
            return False
        
        # Get username
        username = input("Enter your Hugging Face username: ").strip()
        if not username:
            print("❌ Username is required")
            return False
        
        # Repository configuration
        repo_name = f"{username}/aurora-trinity-3"
        print(f"📦 Repository: {repo_name}")
        
        # Initialize API
        api = HfApi(token=hf_token)
        
        # Create repository
        print("\n🔨 Creating repository...")
        try:
            create_repo(
                repo_id=repo_name,
                token=hf_token,
                repo_type="model",
                exist_ok=True,
                private=False
            )
            print(f"✅ Repository created: https://huggingface.co/{repo_name}")
        except Exception as e:
            print(f"⚠️ Repository might already exist: {e}")
        
        # Upload files from aurora_hf_ready directory
        upload_dir = Path("aurora_hf_ready")
        if not upload_dir.exists():
            print(f"❌ Upload directory not found: {upload_dir}")
            return False
        
        print(f"\n📤 Uploading files from {upload_dir}...")
        
        # Upload each file
        files_uploaded = []
        for file_path in upload_dir.rglob("*"):
            if file_path.is_file():
                # Calculate relative path for repo
                relative_path = file_path.relative_to(upload_dir)
                
                print(f"  Uploading: {relative_path}")
                
                try:
                    api.upload_file(
                        path_or_fileobj=str(file_path),
                        path_in_repo=str(relative_path).replace("\\", "/"),  # Ensure forward slashes
                        repo_id=repo_name,
                        token=hf_token
                    )
                    files_uploaded.append(str(relative_path))
                except Exception as e:
                    print(f"    ❌ Failed to upload {relative_path}: {e}")
                    continue
        
        print(f"\n✅ Upload completed!")
        print(f"📁 Files uploaded: {len(files_uploaded)}")
        for file in files_uploaded:
            print(f"  ✓ {file}")
        
        print(f"\n🎉 Aurora Trinity-3 is now available at:")
        print(f"🔗 https://huggingface.co/{repo_name}")
        
        print(f"\n💻 To use Aurora Trinity-3:")
        print(f"pip install git+https://huggingface.co/{repo_name}")
        print(f"from aurora_trinity import Trigate, FractalTensor")
        
        return True
        
    except ImportError:
        print("❌ huggingface_hub not installed. Install with:")
        print("pip install huggingface_hub")
        return False
    except Exception as e:
        print(f"❌ Upload failed: {e}")
        return False

def verify_package():
    """Verify the package is ready for upload."""
    print("🔍 Verifying package...")
    
    # Check upload directory
    upload_dir = Path("aurora_hf_ready")
    if not upload_dir.exists():
        print(f"❌ Upload directory not found: {upload_dir}")
        return False
    
    # Check required files
    required_files = [
        "README.md",
        "config.json",
        "setup.py",
        "trinity_3/__init__.py",
        "trinity_3/core.py"
    ]
    
    missing_files = []
    for file in required_files:
        file_path = upload_dir / file
        if not file_path.exists():
            missing_files.append(file)
        else:
            size = file_path.stat().st_size / 1024
            print(f"  ✅ {file} ({size:.1f}KB)")
    
    if missing_files:
        print(f"❌ Missing files: {missing_files}")
        return False
    
    print("✅ Package verification complete!")
    return True

def main():
    print("🌌 Aurora Trinity-3 - Hugging Face Upload Tool")
    print("=" * 60)
    
    # Verify package
    if not verify_package():
        print("\n❌ Package verification failed. Please run deploy_to_hf.py first.")
        return
    
    print("\n📋 Before uploading, make sure you have:")
    print("  1. A Hugging Face account: https://huggingface.co/join")
    print("  2. A Hugging Face token: https://huggingface.co/settings/tokens")
    print("     (with 'Write' permissions)")
    print()
    
    # Confirm upload
    confirm = input("Ready to upload Aurora Trinity-3 to Hugging Face? (y/N): ").strip().lower()
    if confirm not in ['y', 'yes']:
        print("Upload cancelled.")
        return
    
    # Perform upload
    success = upload_to_huggingface()
    
    if success:
        print("\n🎉 SUCCESS! Aurora Trinity-3 is now on Hugging Face!")
        print("🌟 Share your model with the community!")
    else:
        print("\n❌ Upload failed. Please check the errors above.")

if __name__ == "__main__":
    main()
